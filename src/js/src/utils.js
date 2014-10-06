function serialize(obj) {
	var s = [];
	for (var p in obj) {
		if (obj.hasOwnProperty(p)) {
			s.push(encodeURIComponent(p) + '=' + encodeURIComponent(obj[p]));
		}
	}
	return s.join('&');
}

function getBestImage(images) {
	for (var i = images.length; i > 0; i--) {
		var image = images[i-1];
		if (image.height > 144 && image.width > 144)
			return image.url;
	}
	return images[0].url;
}

function getAge(dateString) {
	var today = new Date();
	var birthDate = new Date(dateString);
	var age = today.getFullYear() - birthDate.getFullYear();
	var m = today.getMonth() - birthDate.getMonth();
	if (m < 0 || (m === 0 && today.getDate() < birthDate.getDate())) {
		age--;
	}
	return age;
}
