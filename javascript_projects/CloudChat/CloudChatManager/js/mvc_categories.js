Array.prototype.removeByIndex = function(index) {
    this.splice(index, 1);
};


/* 
 * 
 * mvc handling of categories pane
 * 
 *
 */
var categoriModel;
var categoriCntrl;
var timeout_widgets;



(function () {
    'use strict';

    function Event(sender) {
        this._sender = sender;
        this._listeners = [];
    }

    Event.prototype = {
        attach : function (listener) {
            this._listeners.push(listener);
        },
        notify : function (args) {
            var index;

            for (index = 0; index < this._listeners.length; index += 1) {
                this._listeners[index](this._sender, args);
            }
        }
    };

    /**
     * The Model. Model stores items and notifies
     * observers about changes.
     */
    function categoriesModel(classitems) {
        this._classitems = classitems;
        this._lastentrytime  = new Date(0); // start point in the 70's
        this._focusElementID = 0; // will point to element in focus
        
        // event handlers
        this.classitemAdded = new Event(this);
        this.classitemRemoved = new Event(this);
        //...
    }

    categoriesModel.prototype = {
    	addClassWidgetInfoItem : function (classItem) {
            var bExists = false;
            //var nVisitors = 0;
            // check incomming info 
            for (i = 0; i < this._classitems.length; i++)
            {
                if (this._classitems[i]._srcAlias === classItem._srcAlias) {
                    bExists = true;
                    // reset the time
                    this._classitems[i]._timereceived = new Date();
                }
                // make check if its lastEntryTime is the current one and if so, then display focus mark 
                var _entrytime = new Date(this._classitems[i]._lastentrytime);
                if(_entrytime > this._lastentrytime) {
                    this._lastentrytime = _entrytime;
                    // hide previouse focus mark and show new
                    this._classitems[this._focusElementID]._bFocus = false;
                    // set new focus
                    this._focusElementID = i;
                    this._classitems[this._focusElementID]._bFocus = true;
                }
            }
            if(bExists === false)
    	    {// new widget connected 
                addNewWidgetComm(classItem._srcHomepageAlias, classItem._srcAlias);
                this._classitems.push(classItem);
                this.classitemAdded.notify({ classItem : classItem });    
                
                // remove timeout entries
                clearInterval(timeout_widgets);
                timeout_widgets=setInterval(function(){
                    if(/loaded|complete/.test(document.readyState)){
                       // remove all entries with timestamp older than x time
                       var currenttime = new Date();
                       for (i = 0; i < categoriModel._classitems.length; i++)
                       {
                           var timeDiff = Math.abs(currenttime.getTime() - categoriModel._classitems[i]._timereceived.getTime());

                           if (timeDiff > 10000) {
                               // timeout occurred for this entry - it will be removed
                               removeWidgetComm(categoriModel._classitems[i]._srcHomepageAlias);
                               categoriModel._classitems.removeByIndex(i);
                               categoriModel.classitemRemoved.notify();
                           }
                      }
                    }
                }, 4000);
                
            }
     	},
	getClassItems : function () {
	   return [].concat(this._classitems); // Just copying the array so that if you modify it after it gets returned then you don't touch the underlying collection.
	},
        
        getItem: function (srcAlias) {
            var strreturn="";
            var i = 0;
            for (i=0; i < this._classitems.length; i++)
            {
                if(this._classitems[i]._srcAlias === srcAlias)
                {
                    // found user
                    strreturn = this._classitems[i]._src;
                    break;
                }
            }
            return strreturn;
        }

    };

    /**
     * The View. View presents the model and provides
     * the UI events. The controller is attached to these
     * events to handle the user interraction.
     */
    function categoriesView(model, elements) {
        this._model = model;
        this._elements = elements;
        var _this = this;

        // events from user
        this.accordionClicked = new Event(this);
      
        // attach model listeners
        this._model.classitemAdded.attach(function () {
            refresh();
        });
        this._model.classitemRemoved.attach(function () {
            refresh();
        });
        this._elements.accordion.click(function(e) {
            //TODO: reset color of previous selected part
            
            //TODO: Find a smart way of - change color of selected part - 
            // e.currentTarget.style.backgroundColor = "rgb(0,128,0)"
            //row.className = "panel panel-warning";

            //e.target.parentElement.parentElement.parentElement.parentElement.style.backgroundColor = "rgb(0,128,0)"; // when pressing the count number it sets pane color

            //e.target.parentElement.parentElement.parentElement.style.backgroundColor = "rgb(0,0,20)";
            
            // notify selection
            _this.accordionClicked.notify(e);
        });
        
     }

    categoriesView.prototype = {
        init: function() {
            init_categoryPage();
        }
    };

    /**
     * The Controller. Controller responds to user actions and
     * invokes changes on the model.
     */
    function categoriesController(model, view) {
        this._model = model;
        this._view = view;
        var _this = this;
        
        this._view.accordionClicked.attach(function(sender, args) {
            _this.setSession(args); // based on accordion list item, then setup the chat room list - from this list the manager choose which question to answer

            // change view to mvc_context.js
            NaviBar("nav_5"); // move to session page
            
        });
        
    }

    categoriesController.prototype = {
        setSession: function(args) {
            // based on accordion list item, then setup the chat room list - from this list the manager choose which question to answer  
//            var id = args.toElement.id;
            var id = args.target.id;
            var nFoundIndex = -1;
            var i=0;
            var ii=0;
            
            //find corresponding widget classitem
            for (i = 0; i < this._model._classitems.length; i++)
            {
                if (this._model._classitems[i]._srcAlias === id) {
                    nFoundIndex = i;
                    break;
                }
            }
            
            if(nFoundIndex !== -1)
            {
                // First clean up - remove last selected sessions if any
                context_cont_this._model.emptySelector();
                context_cont_this._model.removeAllSessions();
                
                // setup session selector with all widget connected on same page, this will give the selector an entry per widget
                for (ii = 0; ii < this._model._classitems.length; ii++)
                {
                    if (this._model._classitems[ii]._srcHomepageAlias == this._model._classitems[nFoundIndex]._srcHomepageAlias) 
                    {
                        // first clear selector
                        context_cont_this._model.emptySelector();

                        var homepage = this._model._classitems[ii]._srcHomepageAlias;
                        var user = this._model._classitems[ii]._srcAlias;
                        var bFocus = this._model._classitems[ii]._bFocus;
                        var title = "ding dong";
                        var question = "roaming";
                        context_cont_this._model.addQuestion(homepage,user,title, question, bFocus); // this lies in mvc_context.js
			SendManagerStatus(bManagerStatus, this._model._classitems[ii]._src, this, callbackChat); // [inside mvc_chat.js] manager should send JSCManagerStatus every x second, when dealing with custormer, however it should first send status to all waiting customers under the choosen category
                     }
                }
                
            }
        }
    };

    $(function () {
        var model = new categoriesModel([]),
            view = new categoriesView(model, {
                'accordion': $('#accordion')
            }),
            categoriCntrl = new categoriesController(model, view);

        view.init();
        categoriModel = model; // used by external .js files
    });
}());
