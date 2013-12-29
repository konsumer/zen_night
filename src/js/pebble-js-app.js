Pebble.addEventListener("ready", function(e) {
	Pebble.showSimpleNotificationOnPebble("Token", Pebble.getAccountToken());
});

Pebble.addEventListener("appmessage", function(e) {
    console.log("Received message: " + e.payload);
});

Pebble.addEventListener("showConfiguration, function(e) {
    Pebble.openURL("http://konsumer.github.io/zen_night/config.html");
});

Pebble.addEventListener("webviewclosed", function(e) {
    var configuration = JSON.parse(e.response);
    console.log("Configuration window returned: ", configuration);
});

