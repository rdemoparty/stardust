function LevelEditor(assetsInstance) {
	var assets = assetsInstance;
	var level = {events: []};

	var addLevel = function() {
		console.log('Adding new level');
		clearLevel();
		scrollLevelToBottom();
	}

	var editLevel = function(name) {
		console.log('Editing level ' + name);
		var levelInfo = assets.levelByName(name);
		$.getJSON('data/' + levelInfo.uri, function(data) {
			level = data;
			levelToDesign();
			scrollLevelToBottom();
		})
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
			.css({
				'top': placementY,
				'left': placementX,
				'width': spriteSheet.spriteWidth,
				'height': spriteSheet.spriteHeight,
				'background-image': 'url(/data/' + spriteSheet.texture + ')',
				'background-position': '-' + spriteSheetFrame.x + 'px' + ' -' + spriteSheetFrame.y + 'px'
			})
			.draggable({containment: "parent"})
			.css('position', 'absolute')
			.appendTo($('#level'));
	}

	var levelFromDesign = function() {
		var levelHeight = $('#level').height();
		level.events = [];

		$('.level-sprite', '#level').each(function() {
			var $this = $(this);
			var recipeName = $this.data('recipe')
			var width = $this.width();
			var height = $this.height();
			var position = $this.position();
			 // div position coords refer to the top-left corner
			var placementX = position.left + width / 2;
			var placementY = position.top + height / 2;

			level.events.push({
				"recipe": recipeName, 
				"x": placementX,
				"y": levelHeight - placementY,
				"layer": 0
			});
		});
	}

	var scrollLevelToBottom = function() {
		$('#level-holder').scrollTop($('#level-holder').prop("scrollHeight"));
	}

	var saveLevel = function(successCallback) {
		levelFromDesign();
		var serializedLevel = JSON.stringify(level, null, 2);
		Utils.saveContentAsFile(serializedLevel, 'levels/level1.json', 'application/json', successCallback);
	}

	var divCoordinatesToGameCoordinates = function(x, y) {
		var levelHeight = $('#level').height();
		return {
			x: x,
			y: levelHeight - y
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
			$.getJSON('/editor/preview-level');
		});
	}

	var createMarkup = function() {
		console.log('Creating recipe editor markup');

		$('#level').on('dblclick', function(e) {
			addNewSpriteAt(e.offsetX, e.offsetY);
			e.preventDefault();
			return false;
		});

		$('#btnPreviewLevel').on('click', function(e) {
			previewLevel();
		})

		scrollLevelToBottom();
	}

	createMarkup();

	return {
		'addLevel': addLevel,
		'editLevel': editLevel
	}
}
