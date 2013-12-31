// load pouchdb
var fileref=document.createElement("script");
fileref.setAttribute("type", "text/javascript");
fileref.setAttribute("src", "http://download.pouchdb.com/pouchdb-nightly.min.js");
var db, configuration;

Pebble.addEventListener("ready", function(e) {
	console.log("zen night JS running.");
	configuration = (window.localStorage.configuration) ? JSON.parse(window.localStorage.configuration) : {};;
	
	console.log('config');
	console.log(JSON.stringify(configuration));
	
	if (configuration.username && configuration.password && configuration.database){
		console.log('db');
		console.log(PouchDB);
		
		db = new PouchDB('zen_night');
		var remote = 'https://' + configuration.username + ':' + configuration.password+'@' + configuration.database,
			opts = {continuous: true};
		db.replicate.to(remote, opts);
		db.replicate.from(remote, opts);
	}else{
		Pebble.showSimpleNotificationOnPebble("Configuration Needed", "Please use the gear icon in the pebble app on your phone to configure the database.");
	}
});

// send all messages to couch
Pebble.addEventListener("appmessage", function(e) {
	// console.log(JSON.stringify(e.payload));
	if (db && configuration.log){
		var record = e.payload;
		record.t = new Date().getTime();
		record.u = Pebble.getAccountToken();
		db.put(record);
	}
});

Pebble.addEventListener("showConfiguration", function(e) {
    Pebble.openURL("http://konsumer.github.io/zen_night/config.html");
});

Pebble.addEventListener("webviewclosed", function(e) {
	window.localStorage.configuration = e.response;
    configuration = JSON.parse(window.localStorage.configuration);
    console.log("Configuration window returned:");
	console.log(JSON.stringify(configuration));
});

