function LevelEditor(assetsInstance) {
	var assets = assetsInstance;
	var level = {events: []};

	var addLevel = function() {
		console.log('Adding new level');
	}

	var editLevel = function(name) {
		console.log('Editing level ' + name);
	}

	var addNewSpriteAt = function(x, y) {

		var recipeName = prompt("Input the name of the recipe");
		var recipe = assets.recipeByName(recipeName);
		var animation = assets.animationByName(recipe.animation);
		var firstFrame = animation.frames[0];
		var spriteSheet = assets.spriteSheetByName(firstFrame.spriteSheet);
		var spriteSheetFrame = spriteSheet.frames[firstFrame.index];

		var spriteWidth = spriteSheet.spriteWidth;
		var spriteHeight = spriteSheet.spriteHeight;
		var placementX = x - spriteWidth / 2;
		var placementY = y - spriteHeight / 2;

		$('<div>')
			.addClass('level-sprite')
			.attr('title', recipeName)
			.css({
				'top': placementY,
				'left': placementX,
				'width': spriteSheet.spriteWidth,
				'height': spriteSheet.spriteHeight,
				'background-image': 'url(/data/' + spriteSheet.texture + ')',
				'background-position': '-' + spriteSheetFrame.x + 'px' + ' -' + spriteSheetFrame.y + 'px'
			})
			.draggable()
			.appendTo($('#level'));

		var levelHeight = $('#level').height();
		level.events.push({
			recipe: recipeName, 
			x: placementX,
			y: levelHeight - placementY,
			layer: 0
		});

		var serializedLevel = JSON.stringify(level, null, 2);
		Utils.saveContentAsFile(serializedLevel, 'levels/level1.json', 'application/json', function() {
			console.log("Level saved");
		});
	}

	var previewLevel = function() {
		$.getJSON('/editor/preview-level');
	}

	var createMarkup = function() {
		console.log('Creating recipe editor markup');

		$('#level').on('dblclick', function(e) {
			console.log(e);
			addNewSpriteAt(e.offsetX, e.offsetY);
		});

		$('#btnPreviewLevel').on('click', function(e) {
			previewLevel();
		})
	}

	createMarkup();

	return {
		'addLevel': addLevel,
		'editLevel': editLevel
	}
}
