// on submit, zip form values & pass back to pebble js
$('form').submit(function(e){
	e.preventDefault();
	var f = $(this).serializeArray();
	var config = _.zipObject(f.map(function(f){ return f.name; }), f.map(function(f){ return f.value; }));
	window.location.href = "pebblejs://close#" + JSON.stringify(config);
});