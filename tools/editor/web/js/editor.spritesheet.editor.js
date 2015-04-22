var SpriteSheetEditor = function(assetsInstance) {

	var assets = assetsInstance;
	var img = new Image();
	var canvas;
	var ctx;
	var selectedFile = null;
	var successCallback;

	var newSpritesheet = function(successCb) {
		successCallback = successCb;

		img = new Image();
		canvas.width = canvas.width;

		selectedFile = null;

		$("#dlgSpriteSheetEditor").dialog({
			width: '1200',
			height: '560',
			modal: true,
			open: function() {
				// initialize canvas?
			},
			close: function() {
			},
			buttons: {
				'Ok': function() {
					var ss = buildSpritesheetFromForm();
					if (isValid(ss)) {
						save(ss);
						$(this).dialog("close");
					}
				},
				'Cancel': function() {
					$(this).dialog("close");
				}
			}
		});
	}

	var buildSpritesheetFromForm = function() {
		return {
			'name': $('#ss-name').val().trim(),
			'spriteWidth': toInt($('#ss-sprite-width').val()),
			'spriteHeight': toInt($('#ss-sprite-height').val()),
			'texture': ''
		}
	}

	var isValid = function(ss) {
		if (selectedFile == null) {
			alert('Select a texture file!');
			$('#ss-file').focus();
			return false;
		}

		if (ss.name == '') {
			alert('Name cannot be empty!');
			$('#ss-name').select().focus();
			return false;
		}

		if (ss.spriteWidth < 1) {
			alert('Invalid sprite width!');
			$('#ss-sprite-width').select().focus();
			return false;
		}

		if (ss.spriteHeight < 1) {
			alert('Invalid sprite height!');
			$('#ss-sprite-height').select().focus();
			return false;
		}

		if (assets.spriteSheetByName(ss.name) != null) {
			alert('A spritesheet with this name already exists!');
			$('#ss-name').select().focus();
			return false;
		}

		return true;
	}

	var save = function(ss) {
		var s = ss;
		s.texture = 'sprites/' + selectedFile.name;
		getFileContent(function() {
			Utils.saveFile(selectedFile, s.texture, function() {
				assets.addSpriteSheet(s);
				assets.saveAnimations();
				if (typeof successCallback !== 'undefined') {
					successCallback(ss);
				}
			});
		});
	}

	var toInt = function(value, defaultValue) {
		try {
			return parseInt(value);
		} catch(e) {
			return defaultValue;
		}
	}

	var redraw = function() {
		ctx.canvas.width = ctx.canvas.width;
		ctx.drawImage(img, 0, 0);

		var ssWidth = toInt($('#ss-sprite-width').val(), 0);
		var ssHeight = toInt($('#ss-sprite-height').val(), 0);

		if (ssWidth > 0 && ssHeight > 0) {
			drawGrid(ssWidth, ssHeight);
		}
	}

	var drawGrid = function(cellWidth, cellHeight) {
		ctx.beginPath();

		var y = 0;
		while (y < ctx.canvas.height) {
			var x = 0;
			while (x < ctx.canvas.width) {
				if (x > 0) {
					ctx.moveTo(x, 0);
					ctx.lineTo(x, ctx.canvas.height);
				}
				x += cellWidth;
			}
			if (y > 0) {
				ctx.moveTo(0, y);
				ctx.lineTo(ctx.canvas.width, y);
			}
			y += cellHeight;
		}

		ctx.strokeStyle = 'rgba(255, 255, 255, 0.9)';
		ctx.stroke();
	}

	var getFileContent = function(callback) {
		var r = new FileReader();
		r.onload = function(event) {
			fileContentRetrieved = true;
			fileContent = event.target.result;
			callback(fileContent);
		}
		r.readAsBinaryString(selectedFile);
	}

	var triggerImageLoad = function() {
		var reader = new FileReader();
		reader.onload = function(event) {
			img.onload = function() {
				canvas.width = img.width;
				canvas.height = img.height;
				redraw();
			}
			img.src = event.target.result;
		}
		reader.readAsDataURL(selectedFile);
	}

	var createMarkup = function() {
		console.log('Creating spritesheet editor markup');
		var $dialog = $('<div>')
							.attr('id', 'dlgSpriteSheetEditor')
							.attr('title', 'Spritesheet Editor')
							.addClass('dialog')
							.zIndex(1000)
							.hide();

		var dialogContent = 
			"<div id=\"ss-editor-holder\">" +
				"<div id=\"ss-editor-sidebar\">" + 
					"<div class=\"row\">" +
						"<div class=\"label\">File</div>" + 
						"<div class=\"field\">" + 
							"<input type=\"file\" id=\"ss-file\" accept=\".png\" />" +
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Name</div>" + 
						"<div class=\"field\">" + 
							"<input type=\"text\" id=\"ss-name\" value=\"\" />" +
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Sprite width</div>" + 
						"<div class=\"field\">" + 
							"<input type=\"text\" id=\"ss-sprite-width\" value=\"64\" />" +
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Sprite height</div>" + 
						"<div class=\"field\">" + 
							"<input type=\"text\" id=\"ss-sprite-height\" value=\"64\" />" +
						"</div>" + 
					"</div>" + 
				"</div>" +
				"<div id=\"ss-canvas-holder\"><canvas id=\"ss-canvas\" /></div>" +
			"</div>";
		$(dialogContent).appendTo($dialog);
		$dialog.appendTo($("body"));

		$('#ss-sprite-height').keyup(function() {
			redraw();
		})
		$('#ss-sprite-width').keyup(function() {
			redraw();
		})

		canvas = document.getElementById('ss-canvas');
		ctx = canvas.getContext('2d');

		$('#ss-file').change(function(e) {
			selectedFile = e.target.files[0];
			console.log(selectedFile);
			triggerImageLoad(selectedFile);
			return false;
		});
	}

	createMarkup();

	return {
		new: newSpritesheet
	};
}
