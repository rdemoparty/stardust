Utils = {

	toInt: function(value, defaultValue) {
		try {
			var i = parseInt(value);
			return isNaN(i) ? defaultValue : i;
		} catch (e) {
			return defaultValue;
		}
	},

	extractDirFromUri: function(uri) {
		var pieces = uri.split('/');
		pieces.pop();
		return pieces.join('/');
	},

	extractFilenameFromUri: function(uri) {
		var pieces = uri.split('/');
		return pieces[pieces.length - 1];
	},

	saveContentAsFile: function(content, uri, mimeType, successCallback) {
		var filename = Utils.extractFilenameFromUri(uri);
		var path = Utils.extractDirFromUri(uri);

		var multipart = new FormData();
		multipart.append('dir', path);
		multipart.append('file', new Blob([content], {type: mimeType}), filename);

		$.ajax({
			'type': 'POST',
			'url': '/data',
			'data': multipart,
			'cache': false,
			'processData': false,
			'contentType': false,
			'success': function() {
				console.log('Success saving ' + path + '/' + filename);
				if (typeof successCallback != 'undefined') {
					successCallback();
				}
			},
			'error': function(xhr, status, error) {
				console.log('Error saving ' + path + filename + '. Error: ' + error);
			}
		});
	},

	saveFile: function(file, uri, successCallback) {
		var filename = Utils.extractFilenameFromUri(uri);
		var path = Utils.extractDirFromUri(uri);

		var multipart = new FormData();
		multipart.append('dir', path);
		multipart.append(file.name, file);

		var xhr = new XMLHttpRequest();
		xhr.open('POST', '/data', true);
		xhr.onload = function(e) {
			if (typeof successCallback != 'undefined') {
				successCallback();
			}
		};

		xhr.send(multipart);  // multipart/form-data
	}
}