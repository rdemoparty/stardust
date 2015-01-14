var Assets = function() {
	this.animations = {};
	this.spritesheets = {};
	this.textures = {};
	this.descriptorsLoaded = false;
	this.resourcesLoadedCallback = null;
	this.texturesToLoad = 0;
	this.recipes = {};
	this.recipesLoaded = false;
}

Assets.prototype.spriteSheetByName = function(name) {
	for (var i in this.spritesheets)
		if (this.spritesheets[i].name == name)
			return this.spritesheets[i];

	return null;
}

Assets.prototype.animationByName = function(name) {
	for (var i in this.animations)
		if (this.animations[i].name == name)
			return this.animations[i];

	return null;
}

Assets.prototype.recipeByName = function(name) {
	for (var i in this.recipes)
		if (this.recipes[i].name == name)
			return this.recipes[i];

	return null;
}

Assets.prototype.addRecipe = function(recipe) {
	this.recipes[this.recipes.length] = recipe;
	this.sortRecipes();
}

Assets.prototype.sortRecipes = function() {
	this.recipes.sort(function(el1, el2) {
		return el1.name == el2.name ? 0 : (el1.name < el2.name ? -1 : 1); 
	});
}

Assets.prototype.addAnimation = function(animation) {
	this.animations[this.animations.length] = animation;
	this.sortAnimations();
}

Assets.prototype.addSpriteSheet = function(ss) {
	this.spritesheets[this.spritesheets.length] = ss;
}

Assets.prototype.sortAnimations = function() {
	this.animations.sort(function(el1, el2) {
		return el1.name == el2.name ? 0 : (el1.name < el2.name ? -1 : 1); 
	});
}

Assets.prototype.calculateSpritesheetFrames = function() {
	for (var i in this.spritesheets) {
		var ss = this.spritesheets[i];
		ss.frames = [];
		var textureWidth = this.textures[ss.texture].width;
		var textureHeight = this.textures[ss.texture].height;
		var spriteWidth = ss.spriteWidth;
		var spriteHeight = ss.spriteHeight;

		var y = 0;
		while (y < textureHeight) {
			var x = 0;
			while (x < textureWidth) {
				ss.frames[ss.frames.length] = {
					'x': x,
					'y': y,
					'w': spriteWidth,
					'h': spriteHeight
				}
				x += spriteWidth;
			}
			y += spriteHeight;
		}
	}
}

Assets.prototype.preloadTexture = function(textureName, takeRaw, finishedCb) {
	this.texturesToLoad++;
	var textureSrc = takeRaw ? '/' + textureName : '/data/' + textureName;
	var t = this.textures;
	var cb = finishedCb;
	var img = $('<img>').load(function() {
		t[textureName] = this;
		console.log('loaded ' + textureName);
		if (typeof cb !== 'undefined') {
			cb(textureName);
		}
	}).attr('src', textureSrc);
}

Assets.prototype.unpackAnimationFrameRanges = function(animation) {
	var newFrames = [];
	for (var i in animation.frames) {
		if ("indexTo" in animation.frames[i]) {
			for (var j = animation.frames[i].index; j <= animation.frames[i].indexTo; j++) {
				newFrames[newFrames.length] = {
					'spriteSheet': animation.frames[i].spriteSheet,
					'index': j
				};
			}
		} else {
			newFrames[newFrames.length] = {
				'spriteSheet': animation.frames[i].spriteSheet,
				'index': animation.frames[i].index
			};
		}
	}
	animation.frames = newFrames;
}

Assets.prototype.preprocessAnimations = function() {
	this.sortAnimations();

	// Replace all indexTo - indexFrom frame ranges with individual frames
	// This will be optimized back before saving.
	for (var i in this.animations)
		this.unpackAnimationFrameRanges(this.animations[i]);
}

Assets.prototype.preloadTextures = function() {
	this.preloadTexture("grid.png", true);
	for (var i in this.spritesheets)
		this.preloadTexture(this.spritesheets[i].texture, false);
}

Assets.prototype.initialize = function(callback) {
	this.resourcesLoadedCallback = callback;
	this.loadResources();
	this.waitForResourcesToLoad();
}

Assets.prototype.loadResources = function() {
	this.descriptorsLoaded = false;
	this.recipesLoade = false;
	var self = this;
	$.getJSON("/data/animations.json", function(data) {
		console.log('Loaded animation descriptors');
		self.spritesheets = data.spriteSheets;
		self.animations = data.animations;
		self.preloadTextures();
		self.preprocessAnimations();
		self.descriptorsLoaded = true;
	});

	$.getJSON("/data/recipes.json", function(data) {
		console.log('Loaded game object descriptors');
		self.recipes = data.recipes;
		self.sortRecipes();
		self.recipesLoaded = true;
	});
}

Assets.prototype.waitForResourcesToLoad = function() {
	var allTexturesLoaded = Object.keys(this.textures).length == this.texturesToLoad;
	if (!this.descriptorsLoaded || !allTexturesLoaded || !this.recipesLoaded) {
		var self = this;
		setTimeout(function() { self.waitForResourcesToLoad(); }, 200);
		return;
	} else {
		// Now that the textures are loaded we can calculate spritesheet frames
		this.calculateSpritesheetFrames();

		console.log('Resources loaded. Executing callback.');
		if (typeof this.resourcesLoadedCallback !== undefined) {
			this.resourcesLoadedCallback();
		}
	}
}


Assets.prototype.getOptimizedAnimationFrames = function(animation) {
	var frames = [];

	var startSpriteSheet = '';
	var startIndex = 0;
	var lastIndex = 0;
	var frame;
	var addNewFrame;

	for (var j in animation.frames) {
		addNewFrame = false;

		if (animation.frames[j].spriteSheet == startSpriteSheet) {
			var nextExpectedIndex = lastIndex + 1;
			if (animation.frames[j].index == nextExpectedIndex) {
				lastIndex = nextExpectedIndex;
				frame['indexTo'] = lastIndex;
			} else {
				addNewFrame = true;
			}
		} else {
			addNewFrame = true;
		}

		if (addNewFrame) {
			frames[frames.length] = {
				'spriteSheet': animation.frames[j].spriteSheet,
				'index': animation.frames[j].index
			}
			frame = frames[frames.length - 1];
			
			startSpriteSheet = animation.frames[j].spriteSheet;
			startIndex = animation.frames[j].index;
			lastIndex = startIndex;
		}
	}

	return frames;
}

Assets.prototype.buildOptimizedAnimations = function() {

	var spritesheets = [];
	for (var i in this.spritesheets) {
		var ss = this.spritesheets[i];
		spritesheets[spritesheets.length] = {
			'name': ss.name,
			'texture': ss.texture,
			'spriteWidth': ss.spriteWidth,
			'spriteHeight': ss.spriteHeight
		}
	}

	var animations = [];

	for (var i in this.animations) {
		var animation = this.animations[i];
		animations[animations.length] = {
			'name': animation.name,
			'frameTimeInMilliseconds': parseInt(animation.frameTimeInMilliseconds),
			'loopType': animation.loopType,
			'frames': this.getOptimizedAnimationFrames(animation)
		}
	}

	return {
		'spriteSheets': spritesheets,
		'animations': animations
	}
}

Assets.prototype.saveRecipes = function() {
	var content = JSON.stringify(this.recipes, null, 2);
	Utils.saveContentAsFile(content, 'recipes.json', 'application/json');
}

Assets.prototype.saveAnimations = function() {
	var content = JSON.stringify(this.buildOptimizedAnimations(), null, 2);
	Utils.saveContentAsFile(content, 'animations.json', 'application/json');
}

Assets.prototype.renameAnimation = function(oldName, newName) {
	var existingAnimation = this.animationByName(oldName);
	if (existingAnimation !== null) {
		existingAnimation.name = newName;
	}

	// fix references
	console.log('TODO: renamed animation from ' + oldName + ' to ' + newName + '. Need to fix recipe references to animations');
}

Assets.prototype.copyAnimationDataFrom = function(animation, name) {
	var existingAnimation = this.animationByName(name);
	if (existingAnimation !== null) {
		existingAnimation.loopType = animation.loopType;
		existingAnimation.frameTimeInMilliseconds = animation.frameTimeInMilliseconds;

		existingAnimation.frames = [];
		for (var i in animation.frames) {
			existingAnimation.frames.push({
				'spriteSheet': animation.frames[i].spriteSheet,
				'index': animation.frames[i].index,
			});

			if (typeof animation.frames[i].indexTo !== undefined) {
				existingAnimation.frames[existingAnimation.frames.length - 1]['indexTo'] = animation.frames[i].indexTo;
			}
		}
	}
}

Assets.prototype.removeAnimation = function(name) {
	for (var i in this.animations) {
		if (this.animations[i].name == name) {
			this.animations.splice(i, 1);
			break;
		}
	}
}