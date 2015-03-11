BUILD := build
#
# NB! IF websocket server has failure or in some way does not disconnect then this script
# will NOT follow after the node test/index.js line !!!! node.js will hang this script - you can manually stop by killing 
# scanvaserver process from another terminal
#
# Run command line tests
# http://sc.tamu.edu/help/general/unix/redirection.html
# http://www.thegeekstuff.com/2011/10/grep-or-and-not-operators/
#
define skeleton_body
/* 
 * 
 * ..... client
 * 
 *
 */


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
    function xxxxModel(items) {
        this._items = items;
	this.bServerLogIn = false;

   	this.x = new Event(this);
	//...
    }

    xxxxModel.prototype = {
        getItems : function () {
            return [].concat(this._items);
        },

	_doit : function (args)
	{
		x.notify();
	}
    };

    /**
     * The View. View presents the model and provides
     * the UI events. The controller is attached to these
     * events to handle the user interraction.
     */
    function xxxxView(model, elements) {
        this._model = model;
        this._elements = elements;

        this.ButtonClicked = new Event(this);

        var _this = this;

        // attach model listeners
	this._model.x.attach(function () {
            _this.justdoit();
	});
     }

     View.prototype = {
        show : function () {
        },

        justdoit : function () {
        }
 	
 
    };

    /**
     * The Controller. Controller responds to user actions and
     * invokes changes on the model.
     */
    function xxxxController(model, view) {
        this._model = model;
        this._view = view;

        var _this = this;

	this._view.ButtonClicked.attach(function (sender, args) {
	    _this.doit(args);
	});
    }

    xxxxController.prototype = {
	doit : function (args) {
	    this._model.doit(args);
	}
    };

    $$(function() {
        xxxx_model_this = new xxxxModel([]),
        xxxx_view_this = new xxxxView(xxxx_model_this, {
                    '<elementid>': $('#<elementid>')
                }),
        xxxx_controller_this = new xxxxController(xxxx_model_this, xxxx_view_this);
        xxxx_model_this.init_xxxx();
        xxxx_view_this.show();
    });

}());
endef
export skeleton_body

all:
	@ echo " ----------------------------------------------------------------------------- "	
	@ echo "  creation of component skeleton in file tmp_comp_skeleton.js - please wait... "	
	@ echo " ----------------------------------------------------------------------------- "	
	@ echo "$$skeleton_body" > tmp_comp_skeleton.js

.PHONY: all 
