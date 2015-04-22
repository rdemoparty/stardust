function LevelEditor(assetsInstance) {
	var assets = assetsInstance;
	var level = {
		events: [], 
		name: '', 
		length: 0, 
		speed: 0
	};
	var levelFilename = '';
	var editDialog = null;
	var entityPropertiesDialog = null;
	var DEFAULT_LEVEL_LENGTH = 50000; // pixels
	var DEFAULT_LEVEL_SPEED = 100; // pixels per second
	var levelModified = false;
	var LAYER_ZERO_ZINDEX = 100;

	var EDITING_MODE = {
		NONE: 0,
		ADD_NEW: 1,
		EDIT_EXISTING: 2
	};
	var editMode = EDITING_MODE.NONE;


	var markLevelAsModified = function() {
		levelModified = true;
		$('#btnSaveLevel').button('enable');
	}

	var markLevelAsSaved = function() {
		levelModified = false;
		$('#btnSaveLevel').button('disable');
	}

	var enablePreviewing = function() {
		$('#btnPreviewLevel').button('enable');
	}

	var addLevel = function() {
		editDialog.dialog({
			width: '600',
			height: '240',
			modal: true,
			open: function() {				
				$('#level-filename').val('');
				$('#level-name').val('');
				$('#level-length').val(DEFAULT_LEVEL_LENGTH); 
				$('#level-speed').val(DEFAULT_LEVEL_SPEED); 
				editMode = EDITING_MODE.ADD_NEW;
			},
			close: function() {
			},
			buttons: {
				'Ok': function() {
					fillLevelDetailsFromDialog();
					if (isLevelValid()) {
						setLevelHeight(level.length);
						clearLevel();
						scrollLevelToBottom();
						$(this).dialog("close");
						markLevelAsModified();
						enablePreviewing();
					}
				},
				'Cancel': function() {
					$(this).dialog("close");
				}
			}
		});
	}

	var isLevelValid = function() {
		if (levelFilename == '') {
			alert('Please specify a level filename');
			$('#level-filename').select().focus();
			return false;
		}

		var levelInfo = assets.levelByName(levelFilename);
		if (levelInfo != null) {
			alert('Level filename already taken. Please specify a new one');
			$('#level-filename').select().focus();
			return false;
		}

		if (level.name == '') {
			alert('Please specify a level name');
			$('#level-name').select().focus();
			return false;
		}
		var minLevelLength = 768;
		if (level.length < minLevelLength) {
			alert('Please specify a valid level length greater than ' + minLevelLength + '!');
			$('#level-length').select().focus();
			return false;
		}

		var minLevelSpeed = 1;
		if (level.speed < minLevelSpeed) {
			alert('Please specify a valid level speed greater than ' + minLevelLength + '!');
			$('#level-speed').select().focus();
			return false;
		}

		return true;
	}

	var fillLevelDetailsFromDialog = function() {
		levelFilename = $('#level-filename').val().trim();
		level.name = $('#level-name').val().trim();
		level.length = Utils.toInt($('#level-length').val(), -1);
		level.speed = Utils.toInt($('#level-speed').val(), -1);
	}

	var setLevelHeight = function(height) {
		$('#level').height(height + 'px');
	}

	var editLevel = function(levelName) {
		console.log('Editing level ' + levelName);
		var levelInfo = assets.levelByName(levelName);
		$.getJSON('data/' + levelInfo.uri, function(data) {
			level = data;
			levelFilename = levelName;
			levelToDesign();
			scrollLevelToBottom();
			markLevelAsSaved();
			enablePreviewing();
		});
	}

	var levelToDesign = function() {
		clearLevel();
		for (var i in level.events) {
			var e = level.events[i];
			addRecipeInstanceAtGameCoordinates(e.recipe, e.x, e.y, e.layer);
		}
	}

	var clearLevel = function() {
		$('.level-sprite', '#level').remove();
	}

	var addRecipeInstanceAtGameCoordinates = function(recipeName, x, y, layer) {
		var recipe = assets.recipeByName(recipeName);
		var animation = assets.animationByName(recipe.animation);
		var firstFrame = animation.frames[0];
		var spriteSheet = assets.spriteSheetByName(firstFrame.spriteSheet);
		var spriteSheetFrame = spriteSheet.frames[firstFrame.index];

		var levelHeight = $('#level').height();

		var spriteWidth = spriteSheet.spriteWidth;
		var spriteHeight = spriteSheet.spriteHeight;
		var placementX = x - spriteWidth / 2;
		var placementY = levelHeight - y - spriteHeight / 2;

		$('<div>')
			.addClass('level-sprite')
			.attr('title', recipeName)
			.data('recipe', recipeName)
			.data('layer', layer)
			.css({
				'top': placementY,
				'left': placementX,
				'width': spriteSheet.spriteWidth,
				'height': spriteSheet.spriteHeight,
				'background-image': 'url(/data/' + spriteSheet.texture + ')',
				'background-position': '-' + spriteSheetFrame.x + 'px' + ' -' + spriteSheetFrame.y + 'px',
			})
			.zIndex(LAYER_ZERO_ZINDEX + layer)
			.draggable({
				containment: "parent",
				stop: function() {
					markLevelAsModified();
				}				
			})
			.css('position', 'absolute')
			.appendTo($('#level'));

		markLevelAsModified();
	}

	var levelFromDesign = function() {
		level.events = [];

		$('.level-sprite', '#level').each(function() {
			var $this = $(this);
			var recipeName = $this.data('recipe')
			var layer = $this.data('layer');
			var position = eventPositionFromEntity(this);

			level.events.push({
				"recipe": recipeName, 
				"x": position.x,
				"y": position.y,
				"layer": layer
			});
		});
	}

	var eventPositionFromEntity = function(entity) {
		$this = $(entity);
		var width = $this.width();
		var height = $this.height();
		var position = $this.position();
		 // div position coords refer to the top-left corner
		var placementX = position.left + width / 2;
		var placementY = position.top + height / 2;
		return {
			'x': placementX,
			'y': level.length - placementY
		}
	}

	var scrollLevelToBottom = function() {
		$('#level-holder').scrollTop($('#level-holder').prop("scrollHeight"));
	}

	var saveLevel = function(successCallback) {
		levelFromDesign();
		var serializedLevel = JSON.stringify(level, null, 2);
		Utils.saveContentAsFile(serializedLevel, 'levels/' + levelFilename, 'application/json', function() {
			markLevelAsSaved();
			if (typeof successCallback != 'undefined')
				successCallback();
		});
	}

	var divCoordinatesToGameCoordinates = function(x, y) {
		return {
			'x': x,
			'y': level.length - y
		}
	}

	var addNewSpriteAt = function(x, y) {
		var recipeName = prompt("Input the name of the recipe");
		var recipe = assets.recipeByName(recipeName);

		var coords = divCoordinatesToGameCoordinates(x, y);
		var layer = 0;
		addRecipeInstanceAtGameCoordinates(recipeName, coords.x, coords.y, layer);
	}

	var previewLevel = function() {
		saveLevel(function() {
			$.getJSON('/editor/preview-level/' + levelFilename);
		});
	}

	var openEntityPropertiesDialog = function(entity) {
		var initialPosition = eventPositionFromEntity(entity);
		var initialLayer = $(entity).data('layer') ? $(entity).data('layer') : 0;

		entityPropertiesDialog.dialog({
			width: '600',
			height: '240',
			modal: true,
			open: function() {				
				$('#entity-x').val(initialPosition.x);
				$('#entity-y').val(initialPosition.y);
				$('#entity-layer').val(initialLayer);
			},
			close: function() {
			},
			buttons: {
				'Ok': function() {
					var x = Utils.toInt($('#entity-x').val(), initialPosition.x);
					var y = Utils.toInt($('#entity-y').val(), initialPosition.x);
					var layer = Utils.toInt($('#entity-layer').val(), initialLayer);

					var dx = x - initialPosition.x;
					var dy = y - initialPosition.y;

					var position = $(entity).position();

					$(entity).css({
						'top': Utils.toInt(position.top) - dy,
						'left': Utils.toInt(position.left) + dx,
					});

					$(entity).zIndex(LAYER_ZERO_ZINDEX + layer);
					$(entity).data('layer', layer);

					markLevelAsModified();
					$(this).dialog('close');
				},
				'Cancel': function() {
					$(this).dialog('close');
				}
			}
		});
	}

	var createMarkup = function() {
		console.log('Creating recipe editor markup');

		createLevelEditDialogMarkup();
		createEntityPropertiesDialogMarkup();

		$('#btnPreviewLevel').on('click', function(e) {
			previewLevel();
			return false;
		});

		$('#btnSaveLevel').on('click', function(e) {
			saveLevel();
			return false;
		});

		$(document).on('dblclick', '.level-sprite', function(e) {
			$('.level-sprite.selected').each(function() {
				$(this).removeClass('selected');
			});
			$(this).addClass('selected');
			openEntityPropertiesDialog(this);
			return false;
		});

		$(document).on('dblclick', '#level', function(e) {
			if (levelFilename != '') {
				addNewSpriteAt(e.offsetX, e.offsetY);
				return false;
			}
		});

		$(document).on('click', '.level-sprite', function(e) {
			$(this).toggleClass('selected');
			return false;
		});

		$(document).on('keydown', '', function(e) {
			if (e.keyCode == $.ui.keyCode.DELETE) {
				var deletedItems = false;
				$('.level-sprite.selected').each(function() {
					$(this).remove();
					deletedItems = true;
				});

				if (deletedItems) {
					markLevelAsModified();
					e.preventDefault();
					return false;
				}
			}
		});

		scrollLevelToBottom();
	}

	var createEntityPropertiesDialogMarkup = function() {
		var $dialog = $('<div>')
							.attr('id', 'dlgEntityProperties')
							.attr('title', 'Entity Properties')
							.addClass('dialog')
							.zIndex(1000)
							.hide();

		var dialogContent = 
			"<div id=\"entity-properties-dialog-holder\">" +
				"<div class=\"row\">" +
					"<div class=\"label\">X</div>" + 
					"<div class=\"field\">" + 
						"<input type=\"text\" id=\"entity-x\" value=\"\" />" +
					"</div>" + 
				"</div>" + 
				"<div class=\"row\">" +
					"<div class=\"label\">Y</div>" + 
					"<div class=\"field\">" + 
						"<input type=\"text\" id=\"entity-y\" value=\"\" />" +
					"</div>" + 
				"</div>" + 
				"<div class=\"row\">" +
					"<div class=\"label\">Layer</div>" + 
					"<div class=\"field\">" + 
						"<input type=\"text\" id=\"entity-layer\" value=\"\" />" +
					"</div>" + 
				"</div>" + 
			"</div>";
		$(dialogContent).appendTo($dialog);
		$dialog.appendTo($("body"));

		$('#entity-properties-dialog-holder input').on('keydown', function(e) {
			if (e.keyCode == $.ui.keyCode.ENTER) {
				$('#dlgEntityProperties').next().find('button:eq(0)').trigger('click');
				return false;
			}
		})

		entityPropertiesDialog = $dialog;
	}

	var createLevelEditDialogMarkup = function() {
		var $dialog = $('<div>')
							.attr('id', 'dlgLevelEditor')
							.attr('title', 'Level Editor')
							.addClass('dialog')
							.zIndex(1000)
							.hide();

		var dialogContent = 
			"<div id=\"level-editor-dialog-holder\">" +
				"<div class=\"row\">" +
					"<div class=\"label\">Filename</div>" + 
					"<div class=\"field\">" + 
						"<input type=\"text\" id=\"level-filename\" value=\"\" />" +
					"</div>" + 
				"</div>" + 
				"<div class=\"row\">" +
					"<div class=\"label\">Name</div>" + 
					"<div class=\"field\">" + 
						"<input type=\"text\" id=\"level-name\" value=\"\" />" +
					"</div>" + 
				"</div>" + 
				"<div class=\"row\">" +
					"<div class=\"label\">Length</div>" + 
					"<div class=\"field\">" + 
						"<input type=\"text\" id=\"level-length\" value=\"\" />" +
					"</div>" + 
				"</div>" + 
				"<div class=\"row\">" +
					"<div class=\"label\">Speed</div>" + 
					"<div class=\"field\">" + 
						"<input type=\"text\" id=\"level-speed\" value=\"\" />" +
					"</div>" + 
				"</div>" + 
			"</div>";
		$(dialogContent).appendTo($dialog);
		$dialog.appendTo($("body"));

		$('#dlgLevelEditor input').on('keydown', function(e) {
			if (e.keyCode == $.ui.keyCode.ENTER) {
				$('#dlgLevelEditor').next().find('button:eq(0)').trigger('click');
				return false;
			}
		})


		editDialog = $dialog;
	}

	createMarkup();

	return {
		'addLevel': addLevel,
		'editLevel': editLevel
	}
}
