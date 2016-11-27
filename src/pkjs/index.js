var appName = "MPA_Skeleton";
var Clay = require('pebble-clay');
var clayConfig = require('./config');
var customClay = require('./custom-clay');
var clay = new Clay(clayConfig, customClay);




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
var xhrRequest = function (method, url, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.onerror = function() {
    console.log("HTTP " + xhr.status + " ERROR: " + xhr.statusText);
  };
  xhr.ontimeout = function() {
    console.log("HTTP TIMEOUT for " + url);
  };
  xhr.timeout = 10000;  // 10 sec
  xhr.open(method, url);
  xhr.send();
};



/////////////////////////////////////////////////////////////////////////////
/// Listen for when an AppMessage is received.
/////////////////////////////////////////////////////////////////////////////
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received: " + JSON.stringify(e.payload));
    var dict = e.payload;

    if ('GET_CUSTOM_STR' in dict) {
      var customStr = dict.GET_CUSTOM_STR;
      console.log("Got custom string (" + customStr + ").");
    } else {
      console.log("Unrecognized app message: " + JSON.stringify(dict));
    }


  }
);


/////////////////////////////////////////////////////////////////////////////
/// Listen for when the watch opens communication and inform the watch that
/// the PebbleKit end of the channel is ready.
/////////////////////////////////////////////////////////////////////////////
Pebble.addEventListener('ready',
  function(e) {
    console.log("PebbleKit JS ready!");
    Pebble.sendAppMessage({"PEBKIT_READY": 1});
  }
);
