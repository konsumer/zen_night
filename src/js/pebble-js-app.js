// load pouchdb
var fileref=document.createElement('script')
fileref.setAttribute("type","text/javascript")
fileref.setAttribute("src", "http://download.pouchdb.com/pouchdb-nightly.min.js");
var db;

var configuration= (localStorage.configuration) ? JSON.parse(localStorage.configuration) : {};

Pebble.addEventListener("ready", function(e) {
	console.log("zen night JS running.");
	
	if (configuration.user && configuration.password && configuration.database){
		db = new PouchDB('zen_night');
		var remote = 'https://' + configuration.user + ':' + configuration.password+'@' + configuration.database,
			opts = {continuous: true};
		db.replicate.to(remote, opts);
		db.replicate.from(remote, opts);
	}else{
		Pebble.showSimpleNotificationOnPebble("Configuration Needed", "Please use the gear icon in the pebble app on your phone to configure the database.");
	}
});

Pebble.addEventListener("appmessage", function(e) {
	// console.log(JSON.stringify(e.payload));
	if (db){
		var record = e.payload;
		record.t = new Date().getTime();
		db.put(record);
	}
});

Pebble.addEventListener("showConfiguration", function(e) {
    Pebble.openURL("http://konsumer.github.io/zen_night/config.html#config=" + JSON.stringify(configuration));
});

Pebble.addEventListener("webviewclosed", function(e) {
    configuration = JSON.parse(e.response);
    console.log("Configuration window returned: ", configuration);
});

