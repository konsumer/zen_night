$('form').submit(function(e){
	e.preventDefault();
	var f = $(this).serializeArray();
	var config = _.object(f.map(function(f){ return f.name; }), f.map(function(f){ return f.value; }));
	console.log(config);
	window.location.href = "pebblejs://close#" + JSON.stringify(config);
});