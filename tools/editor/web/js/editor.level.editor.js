function LevelEditor(assetsInstance) {
	var assets = assetsInstance;
	var level = {
		events: [],
		layers: [],
		name: '', 
		length: 0, 
		speed: 0
	};
	var levelFilename = '';
	var editDialog = null;
	var entityPropertiesDialog = null;
    var addLayerDialog = null;
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

	var currentMousePos = { x: -1, y: -1 };
	$(document).mousemove(function(event) {
		currentMousePos.x = event.pageX;
		currentMousePos.y = event.pageY;
	});

	var markLevelAsModified = function() {
		levelModified = true;
		$('#btnSaveLevel').button('enable');
	};

	var markLevelAsSaved = function() {
		levelModified = false;
		$('#btnSaveLevel').button('disable');
	};

	var enablePreviewing = function() {
		$('#btnPreviewLevel').button('enable');
        $('#btnAddLayer').button('enable');
	};

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
						createStandardLevelLayers();
						clearLevel();
						scrollLevelToBottom();
						$(this).dialog("close");
						markLevelAsModified();
						enablePreviewing();
                        setSelectedLayer(0);
                        rebuildLayersMarkup();
					}
				},
				'Cancel': function() {
					$(this).dialog("close");
				}
			}
		});
	};

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
	};

	var fillLevelDetailsFromDialog = function() {
		levelFilename = $('#level-filename').val().trim();
		level.name = $('#level-name').val().trim();
		level.length = Utils.toInt($('#level-length').val(), -1);
		level.speed = Utils.toInt($('#level-speed').val(), -1);
	};

	var setLevelHeight = function(height) {
		$('#level').height(height + 'px');
	};

	var editLevel = function(levelName) {
		console.log('Editing level ' + levelName);
		var levelInfo = assets.levelByName(levelName);
		$.getJSON('data/' + levelInfo.uri, function(data) {
			level = data;
            console.log("Loaded level " + levelName);
			levelFilename = levelName;
            setSelectedLayer(0);
            initializeCurrentLevelLayers();
			levelToDesign();
			scrollLevelToBottom();
			markLevelAsSaved();
			enablePreviewing();
		});
	};

	var levelToDesign = function() {
		clearLevel();
		for (var i in level.events) {
			var e = level.events[i];
			addRecipeInstanceAtGameCoordinates(e.recipe, e.x, e.y, e.layer);
		}
	};

	var clearLevel = function() {
		$('.level-sprite', '#level').remove();
	};

	var createStandardLevelLayers = function() {
		level.layers = [];
		addLevelLayer("main", 0);

        rebuildLayersMarkup();
	};

    var initializeCurrentLevelLayers = function() {
        rebuildLayersMarkup();
    };

	var addLevelLayer = function(name, depth) {
		level.layers.push({
			"name": name,
			"depth": depth,
			"visible": true
		});
    };

    var rebuildLayersMarkup = function() {
        level.layers.sort(function(el1, el2) {
            return el1.depth == el2.depth ? 0 : (el1.depth < el2.depth ? 1 : -1);
        });

        var $levelLayerHolder = $('#level-layer-holder');
        $levelLayerHolder.empty();

        for (var i in level.layers)
            createLayerMarkup(level.layers[i]).appendTo($levelLayerHolder);
    };

    var createLayerMarkup = function(layer) {
        var $result = $('<div>')
                            .addClass('layer')
                            .data('layer-depth', layer.depth);
        if (layer.selected)
            $result.addClass('selected');

        var $checkbox = $('<input type="checkbox"/>')
                            .prop('checked', layer.visible)
                            .on('click', function() {
                                toggleLayerVisibility(layer.name);
                            });
        $('<div>').addClass('layer-visibility').append($checkbox).appendTo($result);
        $('<div>')
            .addClass('layer-name')
            .text(layer.name)
            .on('click', function() {
                setSelectedLayer(layer.depth);
                rebuildLayersMarkup();
            })
            .appendTo($result);
        $('<div>').addClass('layer-depth').text(layer.depth).appendTo($result);

        return $result;
    };

    var setSelectedLayer = function(layerDepth) {
        for (var i in level.layers)
            level.layers[i].selected = (level.layers[i].depth == layerDepth);
    };

    var toggleLayerVisibility = function(layerName) {
        for (var i in level.layers) {
            var layer = level.layers[i];
            if (layer.name == layerName) {
                level.layers[i].visible = !level.layers[i].visible;
                var zIndexDepth = layer.depth + LAYER_ZERO_ZINDEX;
                $('#level').find('.level-sprite').each(function() {
                    if ($(this).css('z-index') == zIndexDepth)
                        $(this).toggle();
                });
                return;
            }
        }
    };

    var addLayer = function() {
        addLayerDialog.dialog({
            width: '600',
            height: '240',
            modal: true,
            open: function() {
                $('#layer-name').val('');
                $('#layer-depth').val(1);
            },
            close: function() {
            },
            buttons: {
                'Ok': function() {
                    var layer = layerFromDialog();
                    if (isLayerValid(layer)) {
                        addLevelLayer(layer.name, layer.depth);
                        rebuildLayersMarkup();
                        markLevelAsModified();
                        $(this).dialog('close');
                    }
                },
                'Cancel': function() {
                    $(this).dialog('close');
                }
            }
        });
    };

    var layerFromDialog = function() {
        var layerName = addLayerDialog.find('#layer-name').val().trim();
        var layerDepth = Utils.toInt($('#layer-depth').val(), 0);
        return {
            'name': layerName,
            'depth': layerDepth
        }
    };

    var isLayerValid = function(layer) {
        for (var i in level.layers) {
            if (level.layers[i].name == layer.name) {
                alert('A layer with this name already exists!');
                addLayerDialog.find('#layer-name').select().focus();
                return false;
            }

            if (level.layers[i].depth == layer.depth) {
                alert('A layer with this depth already exists!');
                addLayerDialog.find('#layer-depth').select().focus();
                return false;
            }
        }
        return true;
    };

	var addRecipeInstanceAtGameCoordinates = function(recipeName, x, y, layer) {
		var recipe = assets.recipeByName(recipeName);
		var animation = assets.animationByName(recipe.animation);
		var firstFrame = animation.frames[0];
		var spriteSheet = assets.spriteSheetByName(firstFrame.spriteSheet);
		var spriteSheetFrame = spriteSheet.frames[firstFrame.index];

        var $level = $('#level');
		var levelHeight = $level.height();

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
				'background-position': '-' + spriteSheetFrame.x + 'px' + ' -' + spriteSheetFrame.y + 'px'
			})
			.zIndex(LAYER_ZERO_ZINDEX + layer)
			.draggable({
				containment: "parent",
				stop: function() {
					markLevelAsModified();
				}				
			})
			.css('position', 'absolute')
			.appendTo($level);

		markLevelAsModified();
	};

	var levelFromDesign = function() {
		level.events = [];

		$('.level-sprite', '#level').each(function() {
			var $this = $(this);
			var recipeName = $this.data('recipe');
			var layer = $this.data('layer');
			var position = eventPositionFromEntity(this);

			level.events.push({
				"recipe": recipeName, 
				"x": position.x,
				"y": position.y,
				"layer": layer
			});
		});
	};

	var eventPositionFromEntity = function(entity) {
		var $this = $(entity);
		var width = $this.width();
		var height = $this.height();
		var position = $this.position();
		 // div position coordinates refer to the top-left corner
		var placementX = position.left + width / 2;
		var placementY = position.top + height / 2;
		return {
			'x': placementX,
			'y': level.length - placementY
		}
	};

	var scrollLevelToBottom = function() {
        var $levelHolder = $('#level-holder');
		$levelHolder.scrollTop($levelHolder.prop('scrollHeight'));
	};

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
	};

	var addNewSpriteAt = function(x, y) {
		var xx = x;
		var yy = y;
		// rebuild recipe menu if needed
		if ($('#recipe-menu li').length != assets.recipes.length) {
			$('#recipe-menu').empty();
			for (var i in assets.recipes) {
				var recipe = assets.recipes[i];
				$('#recipe-menu').append($('<li>').text(recipe.name));
			}
			$('#recipe-menu li').click(function() {
				$(this).parent().trigger('recipe-selected', $(this).text());
			});
		}

		$('#recipe-menu')
			.off('recipe-selected')
			.on('recipe-selected', function(event, recipeName) {
				var coords = divCoordinatesToGameCoordinates(xx, yy);
				var layer = $('#level-layer-holder').find('.layer.selected').data('layer-depth');
				addRecipeInstanceAtGameCoordinates(recipeName, coords.x, coords.y, layer);
				$(this).hide();
			})
			.css({
				'top': currentMousePos.y - 10,
				'left': currentMousePos.x - 20
			})
			.show();
	};

	var previewLevel = function() {
		saveLevel(function() {
			$.getJSON('/editor/preview-level/' + levelFilename);
		});
	};

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
						'left': Utils.toInt(position.left) + dx
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
	};

	var createMarkup = function() {
		console.log('Creating recipe editor markup');

		createLevelEditDialogMarkup();
		createEntityPropertiesDialogMarkup();
        createAddLayerDialogMarkup();

		$('#btnPreviewLevel').on('click', function(e) {
			previewLevel();
			return false;
		});

		$('#btnSaveLevel').on('click', function(e) {
			saveLevel();
			return false;
		});

        $('#btnAddLayer').on('click', function(e) {
            addLayer();
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

		$(document).on('click', '#level', function(e) {
			var $recipeMenu = $('#recipe-menu');
			if ($recipeMenu.is(':visible')) {
				$recipeMenu.hide();
			}
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
	};

	var createEntityPropertiesDialogMarkup = function() {
		var $dialog = $('<div>')
							.attr('id', 'dlgEntityProperties')
							.attr('title', 'Entity Properties')
							.addClass('dialog')
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
		});

		entityPropertiesDialog = $dialog;
	};

    var createAddLayerDialogMarkup = function() {
        var $dialog = $('<div>')
                        .attr('id', 'dlgAddLayer')
                        .attr('title', 'Add Layer')
                        .addClass('dialog')
                        .hide();

        var dialogContent =
            "<div id=\"add-layer-dialog-holder\">" +
                "<div class=\"row\">" +
                    "<div class=\"label\">Name</div>" +
                    "<div class=\"field\">" +
                        "<input type=\"text\" id=\"layer-name\" value=\"\" />" +
                    "</div>" +
                "</div>" +
                "<div class=\"row\">" +
                    "<div class=\"label\">Depth</div>" +
                    "<div class=\"field\">" +
                        "<input type=\"text\" id=\"layer-depth\" value=\"\" />" +
                    "</div>" +
                "</div>" +
            "</div>";
        $(dialogContent).appendTo($dialog);
        $dialog.appendTo($('body'));

        $('#add-layer-dialog-holder').find('input').on('keydown', function(e) {
            if (e.keyCode == $.ui.keyCode.ENTER) {
                $('#dlgAddLayer').next().find('button:eq(0)').trigger('click');
                return false;
            }
        });

        addLayerDialog = $dialog;
    };

	var createLevelEditDialogMarkup = function() {
		var $dialog = $('<div>')
							.attr('id', 'dlgLayerEditor')
							.attr('title', 'Level Editor')
							.addClass('dialog')
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
		});

		editDialog = $dialog;
	};

	createMarkup();

	return {
		'addLevel': addLevel,
		'editLevel': editLevel
	}
}
