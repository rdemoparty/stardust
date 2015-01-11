Utils = {

	extractDirFromUri: function(uri) {
		var pieces = uri.split('/');
		pieces.pop();
		return pieces.join('/');
	},

	extractFilenameFromUri: function(uri) {
		var pieces = uri.split('/');
		return pieces[pieces.length - 1];
	},

	saveContentAsFile: function(content, uri, mimeType) {
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
				console.log('Success saving ' + path + filename);
			},
			'error': function(xhr, status, error) {
				console.log('Error saving ' + path + filename + '. Error: ' + error);
			}
		});
	}
}