// on load, grab stored values
var config = localStorage.config ? JSON.parse(localStorage.config) : {};

$("[name='username']").val(config.username||"");
$("[name='password']").val(config.password||"");
$("[name='database']").val(config.database||"");
$("[name='log']").prop('checked', config.log);

// on submit, zip form values & pass back to pebble js
$('form').submit(function(e){
	e.preventDefault();
	var f = $(this).serializeArray();
	localStorage.config = JSON.stringify( _.zipObject(f.map(function(f){ return f.name; }), f.map(function(f){ return f.value; })) );
	window.location.href = "pebblejs://close#" + localStorage.config;
});
