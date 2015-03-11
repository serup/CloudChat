/*
 *  Copyright 2013 SCANVA ApS.
 *
 *  This index file is the main testcase scenario for cloudmanager
 *  it is used as the setup of the testcases, via make file "makefile_cygwin_cloudmanager_ws.mak" 
 */

// http://docs.oseems.com/general/application/vim/auto-complete-javascript
// auto-complete JavaScript syntaxes by pressing Ctrl + x followed by Ctrl + o while partially typing JavaScript codes. - See more at: http://docs.oseems.com/general/application/vim/auto-complete-javascript#sthash.8iMXwNkB.dpuf
//
var Hogan = Hogan || require('./lib/hogan')
  , doc = this["document"]


////////////////////////////////////////
// DEFINITIONS
////////////////////////////////////////

function appendText(el, text) {
  var textNode = document.createTextNode(text);
  el.appendChild(textNode);
  el.appendChild(document.createElement('br'));
}

if (!this["output"]) {
  var output = function (s) {
    return doc ? appendText(doc.getElementById('console'), s) : console.log(s);
  };
}
var passed = 0;
var failed = 0;

function isInXML(description, got, expected) {
  if (got === expected) {
    output("<TestCase name=\"" + description + "\" result=\"passed\" assertions_passed=\"1\" assertions_failed=\"0\" expected_failures=\"0\"></TestCase>");
//    ++passed;
  } else {
    output("<TestCase name=\"" + description + "\" result=\"failed\" assertions_passed=\"1\" assertions_failed=\"0\" expected_failures=\"0\"></TestCase>");
//    ++failed;
  }
}

function is(got, expected, msg) {
  if (got === expected) {
    output("OK:   " + msg);
    ++passed;
  } else {
    output("FAIL: " + msg);
    output("Expected |" + expected + "|");
    output("     Got |" + got + "|");
    ++failed;
  }
  isInXML(msg, got, expected);
}

////////////////////////////////////////
// WAIT transition events
////////////////////////////////////////
// http://stackoverflow.com/questions/14845826/how-to-wait-without-blocking-in-javascript
var bAbortwait = false;
function waitForConnection(count){
	count = count + 1;
	if (state.connected == false && count < 3){
		setTimeout(waitForConnection, 1000);
	} else {
		if(bAbortwait==false){
		is(state.connected, true, "testConnectServer return expected");
		o.connected(); // Signal test that connection is ready
		}
		bAbortwait=false; // reset
	}
}

function WaitForConnectionToScanva()
{
	bAbortwait = true;
	is(state.connected, true, "testConnectServer return expected");
	o.connected(); // Signal test that connection is ready
}

function fnReceiveDataframe(data)
{
	var result = -1;
	var method = "";
	var trans_id = 0;
	var action1 = false;
	var action2 = false;
	signalCallbackWasReached();

	if(data != -1){
		var decoder_ptr = DED_PUT_DATA_IN_DECODER(  data, data.length );
		if(typeof decoder_ptr != 'string') 
			result = DED_GET_STRUCT_START(decoder_ptr, "event");
		if(result!=-1) {
			method = DED_GET_METHOD( decoder_ptr, "name" );
			trans_id = DED_GET_USHORT( decoder_ptr, "trans_id" );
			action1 = DED_GET_BOOL( decoder_ptr, "action1" );
			action2 = DED_GET_BOOL( decoder_ptr, "action2" );
			result = DED_GET_STRUCT_END( decoder_ptr, "event" );
		}
	}
	is(result, 1, "testRECEIVE_DATAFRAME return expected");
	o.registerClient(); // signal next test to run
}

function fnReceiveRegisterResponse(callback_this,result)
{
	is(result, 1, "testREGISTER_CLIENT return expected"); 
	o.completed();
}

////////////////////////////////////////
// TESTS
////////////////////////////////////////
var o; // main signal event test object

function testConnectServer()
{
	//ListenToServer(WaitForConnectionToScanva,this,"testscript","22980574","asdfasfd","LOCAL");
	ListenToServer(WaitForConnectionToScanva,this,"DADER2D26BAA30E8AD733671370FECFC","serup","EB23445","ws://127.0.0.1:7798"); // should log in to pre created profile; following files should exists in Database : ENTITIEs: DADER2D26BAA30E8AD733671370FECFC.xml; TOASTs : DADE8B486BAA30E8AD733671370FECFC.xml
	
	var count=0;
	waitForConnection(count);
}

function testDED_GET_STRUCT_END(test)
{
	var result=-1;
	//var trans_id = 1 & 0x000000FF; // unsigned short
	var itrans_id = 1; // unsigned short
	var baction1 = true;
	var baction2 = false;

	var encoder_ptr = DED_START_ENCODER();
	if(encoder_ptr<=0) result = -1;  
	var expected = 1;
	DED_PUT_STRUCT_START(encoder_ptr, "event");
	DED_PUT_METHOD(encoder_ptr, "name", "MusicPlayer");
	DED_PUT_USHORT(encoder_ptr, "trans_id", itrans_id);
	DED_PUT_BOOL(encoder_ptr, "action1", baction1);
	DED_PUT_BOOL(encoder_ptr, "action2", baction2);
	DED_PUT_STRUCT_END(encoder_ptr, "event");
	var DEDobject = {
		encoder_ptr: encoder_ptr,
		uncompresseddata: -1,
		iLengthOfTotalData: -1,
		pCompressedData: -1,
		sizeofCompressedData: -1
	};
	result = DED_GET_ENCODED_DATA(DEDobject);
	if(result>0) result=1;
	//is(result, expected, "DED_GET_ENCODED_DATA return expected");

	var decmprsd = decompress_lzss(DEDobject.pCompressedData,DEDobject.sizeofCompressedData);
	var src = "";
	for(i = 0; i < DEDobject.iLengthOfTotalData; i++) src += DEDobject.uncompresseddata[i]; // convert to string
	//is(decmprsd, src, "decompress_lzss decompress success"); 

	var decoder_ptr = DED_PUT_DATA_IN_DECODER( DEDobject.pCompressedData, DEDobject.sizeofCompressedData );
	//is(decoder_ptr, src, "DED_PUT_DATA_IN_DECODER success"); 
	if(typeof decoder_ptr == 'object') 
		result = DED_GET_STRUCT_START(decoder_ptr, "event");
	else
		result=-1;
	//is(result, 1, "DED_GET_STRUCT_START success"); 
	var method = DED_GET_METHOD( decoder_ptr, "name" );
	//is(method, "MusicPlayer", "DED_GET_METHOD return expected");
	var trans_id = DED_GET_USHORT( decoder_ptr, "trans_id" );
	//is(trans_id, 1, "DED_GET_USHORT return expected");
	var action1 = DED_GET_BOOL( decoder_ptr, "action1" );
	//is(action1, true, "DED_GET_BOOL action1 return expected");
	var action2 = DED_GET_BOOL( decoder_ptr, "action2" );
	//is(action2, false, "DED_GET_BOOL action2 return expected");
	var result = DED_GET_STRUCT_END( decoder_ptr, "event" );
	is(result, 1, "testDED_GET_STRUCT_END return expected"); 
	o.send_dataframe();
}

function testSEND_DATAFRAME()
{
	var result=-1;
	var itrans_id = 1; // unsigned short
	var baction1 = true;
	var baction2 = false;

	var encoder_ptr = DED_START_ENCODER();
	if(encoder_ptr<=0) result = -1;  
	var expected = 1;
	DED_PUT_STRUCT_START(encoder_ptr, "event");
	DED_PUT_METHOD(encoder_ptr, "name", "MusicPlayer");
	DED_PUT_USHORT(encoder_ptr, "trans_id", itrans_id);
	DED_PUT_BOOL(encoder_ptr, "action1", baction1);
	DED_PUT_BOOL(encoder_ptr, "action2", baction2);
	DED_PUT_STRUCT_END(encoder_ptr, "event");
	var DEDobject = {
		encoder_ptr: encoder_ptr,
		uncompresseddata: -1,
		iLengthOfTotalData: -1,
		pCompressedData: -1,
		sizeofCompressedData: -1
	};
	result = DED_GET_ENCODED_DATA(DEDobject);
	if(result>0) result=1;

	var decmprsd = decompress_lzss(DEDobject.pCompressedData,DEDobject.sizeofCompressedData);
	var src = "";
	for(i = 0; i < DEDobject.iLengthOfTotalData; i++) src += DEDobject.uncompresseddata[i]; // convert to string
	//is(decmprsd, src, "decompress_lzss decompress success"); 

var callback = function(data) {
	var result = -1;
	signalCallbackWasReached();
	if(typeof data == 'object')
		result = 1;
	else
		result = -1;
	is(result, 1, "testSEND_DATAFRAME response expected"); 
	o.receive_dataframe(); // signal next test to run
};

	//SendDEDpacket(DEDobject.pCompressedData,function(data){ });
	SendDEDpacket(DEDobject.pCompressedData,callback);
	/*if(DEDobject.sizeofCompressedData > 0)
		result = 1;
	else
		result = -1; 
	is(result, 1, "testSEND_DATAFRAME send expected"); */
}

function testRECEIVE_DATAFRAME()
{
	var result=-1;
	var itrans_id = 1; // unsigned short
	var baction1 = true;
	var baction2 = false;

	var encoder_ptr = DED_START_ENCODER();
	if(encoder_ptr<=0) result = -1;  
	var expected = 1;
	DED_PUT_STRUCT_START(encoder_ptr, "event");
	DED_PUT_METHOD(encoder_ptr, "name", "MusicPlayer");
	DED_PUT_USHORT(encoder_ptr, "trans_id", itrans_id);
	DED_PUT_BOOL(encoder_ptr, "action1", baction1);
	DED_PUT_BOOL(encoder_ptr, "action2", baction2);
	DED_PUT_STRUCT_END(encoder_ptr, "event");
	var DEDobject = {
		encoder_ptr: encoder_ptr,
		uncompresseddata: -1,
		iLengthOfTotalData: -1,
		pCompressedData: -1,
		sizeofCompressedData: -1
	};
	result = DED_GET_ENCODED_DATA(DEDobject);
	if(result>0) result=1;

	var decmprsd = decompress_lzss(DEDobject.pCompressedData,DEDobject.sizeofCompressedData);
	var src = "";
	for(i = 0; i < DEDobject.iLengthOfTotalData; i++) src += DEDobject.uncompresseddata[i]; // convert to string
	//is(decmprsd, src, "decompress_lzss decompress success"); 

	SendDEDpacket(DEDobject.pCompressedData,fnReceiveDataframe);
}

function testREGISTER_CLIENT()
{
	//result = RegisterJSClientWithServer("testscript","johnDoe@test","guest",this,fnReceiveRegisterResponse);
	//
	
	//ListenToServer(WaitForConnectionToScanva,this,"DADER2D26BAA30E8AD733671370FECFC","serup","EB23445","LOCAL"); // should log in to pre created profile; following 
	result = RegisterJSClientWithServer("DADER2D26BAA30E8AD733671370FECFC","serup","EB23445",this,fnReceiveRegisterResponse);
}

function testDummy()
{
  var args = Array.prototype.slice.call(arguments);
  is(1, 1, "testDummy return expected"); 
  o.completed();
}

////////////////////////////////////////
// EVENT HANDLERS
////////////////////////////////////////
var strFunctionNames = 0;
function EventTarget(){
    this._listeners = {};
}

EventTarget.prototype = {

    constructor: EventTarget,

    addListener: function(type, listener){
        if (typeof this._listeners[type] == "undefined"){
            this._listeners[type] = [];
        }

        this._listeners[type].push(listener);
    },

    fire: function(event){
        if (typeof event == "string"){
            event = { type: event };
        }
        if (!event.target){
            event.target = this;
        }

        if (!event.type){  //falsy
            throw new Error("Event object missing 'type' property.");
        }

        if (this._listeners[event.type] instanceof Array){
            var listeners = this._listeners[event.type];
            for (var i=0, len=listeners.length; i < len; i++){
                listeners[i].call(this, event);
            }
        }
    },

    removeListener: function(type, listener){
        if (this._listeners[type] instanceof Array){
            var listeners = this._listeners[type];
            for (var i=0, len=listeners.length; i < len; i++){
                if (listeners[i] === listener){
                    listeners.splice(i, 1);
                    break;
                }
            }
        }
    }
};

/////////////////////////////////////////
// TEST OBJECT
/////////////////////////////////////////
function MyTestObject(){
    EventTarget.call(this);
}
MyTestObject.prototype = new EventTarget();
MyTestObject.prototype.constructor = MyTestObject;
MyTestObject.prototype.foo = function(){
    this.fire("foo");
};
MyTestObject.prototype.connected = function(){
    this.fire("connected");
};
MyTestObject.prototype.test = function(callback){
    callback();
};
MyTestObject.prototype.completed = function(){
    this.fire("completed");
};
MyTestObject.prototype.send_dataframe = function(){
    this.fire("send_dataframe");
};
MyTestObject.prototype.receive_dataframe = function(){
    this.fire("receive_dataframe");
};
MyTestObject.prototype.registerClient = function(){
    this.fire("registerClient");
};
MyTestObject.prototype.test_dummy = function(){
    this.fire("test_dummy");
};
function START_RUN_AFTER_TESTS()
{
	testConnectServer(); // first test -- will make a domino effect to start other tests
	
}
function INIT_RUN_AFTER_TEST()
{
	 o = new MyTestObject();
}
// how to use:
// RUN_TEST_AFTER_EVENT("connected", testDED_GET_STRUCT_END, 1); // arguments at the end ....
// RUN_TEST_AFTER_EVENT("completed", complete);
//
// as can be seen, the arguments are added after !
function RUN_TEST_AFTER_EVENT(evt, callback)
{
	o.addListener(evt,function(){o.test(function(){callback(arguments);});});
}




////////////////////////////////////////
// MAIN
////////////////////////////////////////

function complete() {
  output("--------------");
  output("\nTests Complete");
  output("--------------");
  output("Passed: " + passed);
  output("Failed: " + failed);
  output("--------------");
  output("\n");
  shutdownserverconnection();
}

// How to debug :
// https://developers.google.com/chrome-developer-tools/docs/javascript-debugging#breakpoints
function runTests() {
  output("--------------------------");
  output("Test of cloudchatmanager");
  output("--------------------------");
  INIT_RUN_AFTER_TEST(); // NB! Each test needs to signal next unittest, ex. o.test_dummy(); 
  RUN_TEST_AFTER_EVENT("connected", testDED_GET_STRUCT_END);
////  RUN_TEST_AFTER_EVENT("connected", testREGISTER_CLIENT);
  RUN_TEST_AFTER_EVENT("send_dataframe", testSEND_DATAFRAME);
  RUN_TEST_AFTER_EVENT("receive_dataframe", testRECEIVE_DATAFRAME);
  RUN_TEST_AFTER_EVENT("registerClient", testREGISTER_CLIENT);
  RUN_TEST_AFTER_EVENT("completed", complete);
  START_RUN_AFTER_TESTS(); // start test, by running first part -- it should riple down the chain, trickering all tests
}

if (doc) {
  window.onload = runTests;
} else {
  runTests();
}
