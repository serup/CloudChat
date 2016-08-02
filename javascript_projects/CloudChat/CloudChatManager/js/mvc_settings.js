/* 
 * 
 * MVC SETTINGS
 * 
 *
 */

var settings_model_this;
var settings_view_this;
var settings_controller_this;
var timeout_url_foto;
var imgUrl;

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
    function settingsModel(items) {
        this._items = items;
        this.objSettings; // contains value of retrieved settings from database
        this._animateTimer = 0;
        this.myfile = 0;
        this.animateupdate              = new Event(this);           // event fire when animate icon is updated
        this.updateSettingsInDatabase   = new Event(this);  // event fire when settings has been edited
        
        // attach model listeners
        // attach to login model for login notification
        login_model_this.serverLogedIn.attach(function(e) { 
            // send a request to server for profile settings
            // server will send back response dataframe with settings 
            // received settings will then be put into the html settings view
            // this is only done on startup, thus the need for wait until actual login has been done
            
            // send a request to server scanvatar profile, asking it to send its settings'
            SendProfileRequest();
        });
        
        this.updateSettingsInDatabase.attach(function(e, args) {
            // settings element was changed, now send update to database
            var value = e.objSettings.getItem(args.id,"string"); // first check if value actually have changed
            if(args.value != value)
            {
                // change in internal table
                if(e.objSettings.changeItem(args.id, args.value) == true)
                {
                    // create request
                
                    // change in database, by sending request
                    
                }
            }
            
        });
    }

    settingsModel.prototype = {
        getItems : function () {
            return [].concat(this._items);
        },
	init_settings : function () {
            // start update_animate_chat icon -- it will be stopped in _ReceiveChatResponse
            this.animateupdate.notify({_waitIcon: true});
            
            
	}
    };

    /**
     * The View. View presents the model and provides
     * the UI events. The controller is attached to these
     * events to handle the user interraction.
     */
    function settingsView(model, elements) {
        this._model = model;
        this._elements = elements;
	    this.$bBusy=false; // must be set to true when settings are being fetched from server

        // event listeners
        this.ProfileImageClicked = new Event(this);     // user clicks on profile image
        this.UploadImageBtnClicked = new Event(this);   // user clicks on button for upload image
        this.SettingsElementClicked = new Event(this);  // user clicks on a setting element for editing
        this.SettingsElementExit = new Event(this);     // when edit of settings element is finished
        
        var _this = this;
        
        // attach listeners to HTML controls  -- NB! since settings are dynamically loaded, then normal listeners are not possible, in its initial stage - usage of onclick with notify is done instead for initial stage    
        this._elements.profileimage.click(function() {
            _this.ProfileImageClicked.notify('USER');
        });
        

       // attach model listeners
        this._model.animateupdate.attach(function(sender, args) {
           // if true then start timer animation and if false then stop
            var $icon = _this._elements.SettingsUpdateAnimateIcon;
            var animateClass = "glyphicon-refresh-animate";
            if (args._waitIcon === false) {
//                clearTimeout(_this._model._animateTimer);
                $icon.removeClass(animateClass);
                $icon[0].style.display = "none";
                //TODO: take info from received dataframe (args._classitemobj [classProfileSettingsInfoItemObject]) and put in html
               
                if(args._bProfileFound === false)    
                    _this.accessNotPossible();
                else {
                    var UniqueIdFromSettings = args._classitemobj.getItem("profileID","string");
                    
                    // if uniqueId is different from settings Id then reconnect with uniqueId stored in settings - meaning that connection participant entry will be updated with this new ID - previous ID is a random guid
                    if (FetchThisStateUniqueId() !== UniqueIdFromSettings) // this.state.uniqueId of this instance
                    {
                        login_controller_this.reconnect(UniqueIdFromSettings); // This should change state.uniqueId and send a reconnect so server can update socket participant list with new id - if this is not done then cloudmanager will be unknown to system
                    }
		            else
                        _this.show_profile(args._classitemobj); // only show profile settings when profile is logged in
			    
                }
            }
            else {
                $icon[0].style.display = "";
                $icon.addClass(animateClass);
                //+ setTimeout is to indicate some async operation
//                _this._model._animateTimer = window.setTimeout(function() {
//                    $icon.removeClass(animateClass);
//                    $icon[0].style.display = "none";
//                    //TODO: temp test - just show what we have default setup - should be void when real data has been received and parsed
//                    _this.show();
//                }, 8000); // timeout after max 
                //-
            }
        });
  
       
     }

     settingsView.prototype = {
        show : function () {
		var ulsettings;
		if(this.$bBusy === false)
		{
			ulsettings = this._elements.ulsettings;
			ulsettings.html('');
			ulsettings.append($('<li <p style="text-align:center;" ></p><div id="ProfilePage" ><div id="RightCol"><div id="Photo"><img src=" data:image/jpeg;base64,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" onerror="this.onerror=null;this.src="images/user.png";" height="89" width="89" alt="User Avatar" class="img-square"></div><div id="ProfileOptions">a</div></div><div id="Info"><p><strong>First Name:</strong><span>Johnny</span></p><p><strong>Last Name:</strong><span>Serup</span></p><p><strong>Address 1:</strong><span>...</span></p><p><strong>Address 2:</strong><span>...</span></p><p><strong>City:</strong><span>...</span></p></div><!-- Needed because other elements inside ProfilePage have floats --> <div style="clear:both"></div></div> </li>'));
		}


        },
        show_profile : function (ClassItemObj) {
            settings_model_this.objSettings = ClassItemObj; // update model with profile settings data - typically show_profile is called when receiving data from database
             var ulsettings;
            ulsettings = this._elements.ulsettings;
            ulsettings.html('');
    /// Helpful script to cut attributes out of DD files
    /// $cat DD_PROFILE_TOAST_ATTRIBUTES.xml | grep -e '<PhysicalDataElementName>' | sed -e 's/<PhysicalDataElementName>/\"/' | sed -e 's/<\/PhysicalDataElementName>/\",/'
            var strusername =                  settings_model_this.objSettings.getItem("username","string");
            var UniqueIdFromSettings =         settings_model_this.objSettings.getItem("profileID","string");
            var strfirstname =                 settings_model_this.objSettings.getItem("firstname","string");
            var strlastname =                  settings_model_this.objSettings.getItem("lastname","string");
            var strlifecyclestate =            settings_model_this.objSettings.getItem("lifecyclestate","string");    
            //var strusername =                  settings_model_this.objSettings.getItem("username","string");
            var strpassword =                  settings_model_this.objSettings.getItem("password","string");
            var strdevicelist =                settings_model_this.objSettings.getItem("devicelist","string");
            var strfirstname =                 settings_model_this.objSettings.getItem("firstname","string");
            var strlastname =                  settings_model_this.objSettings.getItem("lastname","string");
            var strstreetname =                settings_model_this.objSettings.getItem("streetname","string");
            var strstreetno =                  settings_model_this.objSettings.getItem("streetno","string");
            var strpostalcode =                settings_model_this.objSettings.getItem("postalcode","string");
            var strcity =                      settings_model_this.objSettings.getItem("city","string");
            var strstate =                     settings_model_this.objSettings.getItem("state","string");
            var strcountry =                   settings_model_this.objSettings.getItem("country","string");
            var stremail =                     settings_model_this.objSettings.getItem("email","string");
            var strmobilephone =               settings_model_this.objSettings.getItem("mobilephone","string");
            var strtitle =                     settings_model_this.objSettings.getItem("title","string");
            var strabout =                     settings_model_this.objSettings.getItem("about","string");
            var strfoto =                      settings_model_this.objSettings.getItem("foto","image");
            var strtype =                      settings_model_this.objSettings.getItem("type","string");
            var strstatus =                    settings_model_this.objSettings.getItem("status","string");
            var strexpiredate =                settings_model_this.objSettings.getItem("expiredate","string");
            var strcreditor =                  settings_model_this.objSettings.getItem("creditor","string");
            var strterms =                     settings_model_this.objSettings.getItem("terms","string");
            var strsubscriptions =             settings_model_this.objSettings.getItem("subscriptions","string");
            var strpaymentdetails =            settings_model_this.objSettings.getItem("paymentdetails","string");
            var strnotificationlevel =         settings_model_this.objSettings.getItem("notificationlevel","string");
            var strbankaccount =               settings_model_this.objSettings.getItem("bankaccount","string");
            var strmemberships =               settings_model_this.objSettings.getItem("memberships","string");
            var strfriendships =               settings_model_this.objSettings.getItem("friendships","string");
            var strpresence =                  settings_model_this.objSettings.getItem("presence","string");
            var strlocation =                  settings_model_this.objSettings.getItem("location","string");
            var strcontext =                   settings_model_this.objSettings.getItem("context","string");
            var strschedule =                  settings_model_this.objSettings.getItem("schedule","string");
            var strdeviceID =                  settings_model_this.objSettings.getItem("deviceID","string");
            var strexperience =                settings_model_this.objSettings.getItem("experience","string");
            var strskills =                    settings_model_this.objSettings.getItem("skills","string");
            var strmessagesboard =             settings_model_this.objSettings.getItem("messagesboard","string");
            var streventlog =                  settings_model_this.objSettings.getItem("eventlog","string"); 
            var strsupervisor =                settings_model_this.objSettings.getItem("supervisor","string"); 

            var _htmlfoto = "";
            if(strfoto === "<empty>")
            {
                  _htmlfoto = '<output style="padding-top:0px" id="placeholderProfileFoto"><a  style="color:Orange; font-family:Lucida Sans Unicode; font-size: 180px;z-index:2;"><i id="profileimg" class="scnva scnva-user text-muted" onclick="settings_view_this.ProfileImageClicked.notify('+ "'USER'" +');"></i></a></output>'
                  + '<span id="droppedimage_icon" class="glyphicon glyphicon-plus-sign" style="font-size: 20px; opacity: 0.8; color:whitesmoke ; position: absolute; left:20px; top: 20px;z-index: 0;" onclick="settings_view_this.ProfileImageClicked.notify('+ "'USER'" +');"></span>';
            }
            else {
                _htmlfoto = '<output style="padding-top:0px" id="placeholderProfileFoto"><span><img id="profileimg" style="position: absolute;margin-top: 0px; height: 100%; width: 100%" onclick="settings_view_this.ProfileImageClicked.notify('+ "'USER'" +');" src="'+ strfoto +'" title="Avatar"></span></output>'
                + '<span id="droppedimage_icon" class="glyphicon glyphicon-plus-sign" style="font-size: 20px; opacity: 0.8; color:whitesmoke ; position: absolute; left:20px; top: 20px;z-index: 0;" onclick="settings_view_this.ProfileImageClicked.notify('+ "'USER'" +');"></span>';
            }

            ulsettings.append($('<div id="ProfilePage" >'
                                + '<div id="Photo" class="imageWrapper" style="position: relative; width: 180px; height: 225px;margin-top:0px;border: 2px solid;">'
                                + _htmlfoto
                                + '</div>'
                                + '<table style="width: 100%;  margin-bottom: 80px;">'
                                + '  <tr>'
                                + '     <td ><h3 style="margin-top: 10px; margin-right: 5px; color:darkgray;"><i class="glyphicon glyphicon-user"></i></h3></td><td><b>UserName : </b><span id="_username" onclick="settings_view_this.SettingsElementClicked.notify('+ "'_username'" +');">'+strusername+'</span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '     <td ><h3 style="margin-top: 10px; margin-right: 5px; color:darkgray;"><i class="glyphicon glyphicon-lock"></i></h3></td><td><b>Password : </b><span id="_username" onclick="settings_view_this.SettingsElementClicked.notify('+ "'_username'" +');">'+strpassword+'</span></td>'
                                + '  </tr>'
				                + '  <tr>'
                                + '     <td><h3 style="color:darkgray;"><i style="font-size: 35px;" class="fa fa-male"></i></h3></td><td><b>Name : </b><span id="fullName_">'+ strfirstname + " " + strlastname +'</span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '     <td><h3 style="color:darkgray;"><i class="glyphicon glyphicon-home"></i></h3></td><td><b>Address : </b><span id="userAddress">'+ strstreetname  + " " +  strstreetno + " , " + strpostalcode  + " " +  strcity +'</span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '     <td><h3 style="color:darkgray;"><i class="glyphicon glyphicon-flag"></i></h3></td><td><b>Nationality : </b><span id="userNationality">'+ strcountry  + " (" + strstate + ")" +'</span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '     <td><h3 style="color:darkgray;"><i class="glyphicon glyphicon-earphone"></i></h3></td><td><b>Phone : </b><span id="userPrimaryPhone">'+ strmobilephone +'</span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '     <td><h3 style="color:darkgray;"><i class="glyphicon glyphicon-comment"></i></h3></td><td><b>Note: </b><span id="userAbout">'+ strabout +'</span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '     <td><h3 style="color:darkgray;"><i>@</i></h3></td><td><b>Email : </b><span id="userEmail">'+ stremail +'</span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '    <td><h3 style="color:darkgray;"><i class="glyphicon glyphicon-info-sign"></i></h3></td><td><b>About : </b><span id="userInfo"></span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '    <td><h3 style="color:darkgray;"><i class="glyphicon glyphicon-barcode"></i></h3></td><td><b>ID : </b>'+ UniqueIdFromSettings.small() +'<span id="userID"></span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '    <td><h3 style="color:darkgray;"><i class="glyphicon glyphicon-time"></i></h3></td><td><b>Subscription Expires : </b><span id="userCreationDate"></span></td>'
                                + '  </tr>'
                                + '  <tr>'
                                + '    <td><h3 style="color:darkgray;"><i class="glyphicon glyphicon-barcode"></i></h3></td><td><b>ID : </b>'+ strsupervisor.small() +'<span id="supervisor"></span></td>'
                                + '  </tr>'
                                + '</table>'
                                + '</div>'));
                                

                    // Refresh image 
                    clearInterval(timeout_url_foto);
                    var maxcount=0;
                    timeout_url_foto=setInterval(function(){
                        if(/loaded|complete/.test(document.readyState)){
                            // refresh the profileimg foto - hack - adding time to ignore cash
                            //get the src attribute
                            var source = ulsettings.context.getElementById('profileimg').src;
                            //remove previously added timestamps
                            var new_source = "";
                            source = source.split("?", 1);//turns "image.jpg?timestamp=1234" into "image.jpg" avoiding infinitely adding new timestamps
                            if (/data:image/i.test(source))
                            {
                                // since image was NOT extracted and is still an embedded image, then due to the issue around adding timestamp to embedded images, then do NOT add timestamp
                                new_source = source;
                            }
                            else {
                                //prep new src attribute by adding a timestamp
                                new_source = source + "?timestamp="  + new Date().getTime();
                            } 
                            //alert(new_source); //you may want to alert that during developement to see if you're getting what you wanted
                            //set the new src attribute
                            $('#profileimg').removeAttr("src").attr("src", new_source);
                            if(maxcount>3)
                                clearInterval(timeout_url_foto); // stop refresh
                            maxcount++;
                        }
                    }, 3000);

           
       },
       accessNotPossible : function () {
            var ulsettings;
            ulsettings = this._elements.ulsettings;
            ulsettings.html('');
            ulsettings.append($('<li <div id="ProfilePage" ><div id="RightCol"><div id="Photo"><img src="data:image/jpeg;base64,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" onerror="this.onerror=null;this.src="images/user.png";" height="89" width="89" alt="User Avatar" class="img-square"></div><div id="ProfileOptions">Profile access currently not possible</div></div></li>'));
        }
    };

    /**
     * The Controller. Controller responds to user actions and
     * invokes changes on the model.
     */
    function settingsController(model, view) {
        this._model = model;
        this._view = view;
        
        // event listeners
        var _this = this;
        
        // attach view listeners
        this._view.ProfileImageClicked.attach(function( e ) {
            _this.addPhoto( 'select' );
        });
        this._view.UploadImageBtnClicked.attach(function( e ) {
            _this.handleFileSelect( e );
        });
        this._view.SettingsElementClicked.attach(function( e, args ) {
            _this.editSettingsElement( args );
        });
        this._view.SettingsElementExit.attach(function ( e, args){
            _this.exitEditSettingsElement( args ); // when finished editing setting element then notify model to update database 
        });

    }

    settingsController.prototype = {
        initCamera: function() {
            document.getElementById("camflow_validate").style.display = "none";

            // Grab elements, create settings, etc.
            var canvas = document.getElementById("canvas"),
                    context = canvas.getContext("2d"),
                    video = document.getElementById("video"),
                    videoObj = {"video": true},
            errBack = function(error) {
                console.log("Video capture error: ", error.code);
            };

            // Put video listeners into place
            if (navigator.getUserMedia) { // Standard
                navigator.getUserMedia(videoObj, function(stream) {
                    video.src = stream;
                    video.play();
                }, errBack);
            } else if (navigator.webkitGetUserMedia) { // WebKit-prefixed
                navigator.webkitGetUserMedia(videoObj, function(stream) {
                    video.src = window.webkitURL.createObjectURL(stream);
                    video.play();
                }, errBack);
            }
            else if (navigator.mozGetUserMedia) { // Firefox-prefixed
                navigator.mozGetUserMedia(videoObj, function(stream) {
                    video.src = window.URL.createObjectURL(stream);
                    video.play();
                }, errBack);
            }

            //TODO: should be moved to "attach listeners to HTML controls", since these are actually view events, thus should be in view part of MVC and controller should attach to these events -
            document.getElementById("snap").addEventListener("click", function() {
                //context.drawImage(video, 0, 0, 180, 220);
                context.drawImage(video, 0, 0, canvas.width, canvas.height);

                /// step 1 - create off-screen canvas
                var oc   = document.createElement('canvas'),
                octx = oc.getContext('2d');
                oc.width  = video.width  * 0.5;
                oc.height = video.height * 0.5;
                octx.drawImage(video, 0, 0, oc.width, oc.height); 

                /// step 2
                octx.drawImage(oc, 0, 0, oc.width * 0.9, oc.height * 0.9);

                /// step 3
                context.drawImage(oc, 0, 0, oc.width * 0.9, oc.height * 0.9, 0, 0, canvas.width,   canvas.height);


                document.getElementById("camflow_snap").style.display = "none";
                document.getElementById("camflow_validate").style.display = "inline";
                document.getElementById("camflow_validate_upload").addEventListener("click", function() {
                    imgUrl = canvas.toDataURL();
                    // Render thumbnail.
                    var span2 = document.createElement('span');
                    span2.innerHTML = ['<img id="profileimg" style="position: absolute;margin-top: 0px; height: 100%; width: 100%" onclick="settings_view_this.ProfileImageClicked.notify(' + "'USER'" + ');" src="', imgUrl,
                    '" title="profile foto"/>'].join('');
                    document.getElementById('placeholderProfileFoto').innerHTML = "";
                    document.getElementById('placeholderProfileFoto').insertBefore(span2, null);
                    settings_controller_this.uploadFile();
                });
                document.getElementById("camflow_validate_capture").addEventListener("click", function() {
                    document.getElementById("camflow_snap").style.display = "inline";
                    settings_controller_this.addPhoto('capture');
                });
            });
        },
        
        addPhoto: function(arg) {
            // Check for the various File API support.
            if (window.File && window.FileReader && window.FileList && window.Blob) {
                // Great success! All the File APIs are supported.
                if (arg === "select") {
                    document.getElementById("modalSelection").style.display = "inline";
                    document.getElementById("selectfile").style.display = "none";
                    document.getElementById("capturePhoto").style.display = "none";
                    $('#photoModal').modal();
                }
                if (arg === "upload") {
                    document.getElementById("modalSelection").style.display = "none";
                    document.getElementById("selectfile").style.display = "inline";

                    document.getElementById('files').addEventListener('change', settings_controller_this.handleFileSelect, false);

                    $('#photoModal').modal('hide');
                    $('#files').trigger('click');

                }
                if (arg === "capture") {
                    document.getElementById("modalSelection").style.display = "none";
                    document.getElementById("capturePhoto").style.display = "inline";
                    this.initCamera();
                }
            } else {
                alert('The File APIs are not fully supported in this browser.');
            }
        },
        
        uploadFile: function() {

            $('#photoModal').modal('hide'); // hide the camera modal

            // update profile in backend database
            var ClassItemObj = new classProfileSettingsInfoItemObject();
            ClassItemObj.putItem("foto", this._view._elements.ulsettings.context.getElementById('profileimg').src)
            //ClassItemObj.putItem("foto", imgUrl);
            UpdateProfileOnServer(ClassItemObj);  
        },

       
        handleFileSelect: function (evt) {
            var files = evt.target.files; // FileList object

            if (files[0] === undefined) {
                // the user has clicked on cancel, so hide the photo modal!
                $('#photoModal').modal('hide');
            }
            else {
                //.... the user has choosen an image file
                settings_model_this.myfile = files[0];

                // Loop through the FileList and render image files as thumbnails.
                for (var i = 0, f; f = files[i]; i++) {

                    // Only process image files.
                    if (!f.type.match('image.*')) {
                        continue;
                    }

                    var reader = new FileReader();

                    // Closure to capture the file information.
                    reader.onload = (function(theFile) { // lambda function
                        return function(e) {
          			    // now resize image so it becomes smaller
		        	    var image = new Image();
			            var urlresult = "";
			            image.src = e.target.result; // path to image
			            image.onload = function () {
        				    //TODO: urlresult = resizeMe(image); // should return a data URL for resized image

        				    // Render thumbnail.
	        			    var span2 = document.createElement('span');
		        		    span2.innerHTML = ['<img id="profileimg" style="position: absolute;margin-top: 0px; height: 100%; width: 100%" onclick="settings_view_this.ProfileImageClicked.notify(' + "'USER'" + ');" src="', e.target.result,
				            '" title="', escape(theFile.name), '"/>'].join('');
				            document.getElementById('placeholderProfileFoto').innerHTML = "";
				            document.getElementById('placeholderProfileFoto').insertBefore(span2, null);

				            // upload the image and store in database file 
				            settings_controller_this.uploadFile();
			                };
			            };
		            })(f);

                    // Read in the image file as a data URL.
                    reader.readAsDataURL(f);
                }
            }
        },
        
        editSettingsElement: function (id) {
            var element = document.getElementById(id);
            if (element.tagName !== "INPUT") {
                element.outerHTML = "<input id='" + id + "' maxlength='25' style='color: red; border:none'  type='text' onblur='settings_view_this.SettingsElementExit.notify(\x22" + id + "\x22);' value=\x22" + element.innerHTML + "\x22 />";
                element.focus();
            }
        },
        exitEditSettingsElement: function (id) {
            var element = document.getElementById(id);
            var val = element.value;
            // restore settings tagname
            element.outerHTML = "<span id='" + id + "'></span>";
            document.getElementById(id).innerHTML = val;
            var trimmed_id = id.replace(/_/g, '');
            // notify model to update new value in database
            settings_model_this.updateSettingsInDatabase.notify({id:trimmed_id, value:val});
        },
        fetchSettingsElement: function (name) {
            var value;
            value = settings_model_this.objSettings.getItem(name,"string");
            return value;
        },
        
        fetchSupervisorID: function () {
            var id; 
            //id = "754d148d0522659d0ceb2e6035fad6a8"; // TEST
            //id = fetchSettingsElement("supervisor");
	    id = settings_model_this.objSettings.getItem("supervisor","string"); 
            return id;
        },
        
        fetchAlias: function () {
            var alias; 
            alias = settings_model_this.objSettings.getItem("username","string");
            return alias;
        }
    };
    
    $(function() {
        settings_model_this = new settingsModel([]),
        settings_view_this = new settingsView(settings_model_this, {
			'ulsettings': $('#ulsettings'),
                        'SettingsUpdateAnimateIcon': $('#update_animate_settings'),
                        'profileimage': $('#profileimg') // does not work - using onclick with notify() in html until solution found - reason is that profileimg is not present on init state, it comes first after profile has been fetchecd
                }),
        settings_controller_this = new settingsController(settings_model_this, settings_view_this);
        settings_model_this.init_settings();
    });
}());
