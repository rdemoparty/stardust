var Editor = {
	assets: null,
	animationEditor: null,
	spriteSelector: null,
	spriteSheetEditor: null,
	recipeEditor: null,
	levelEditor: null,

	initialize: function() {
		Editor.createMarkup();

		Editor.assets = new Assets();
		Editor.assets.initialize(function() {
			console.log('Assets initialized');
			Editor.populateAnimationList();
			Editor.populateRecipeList();
			Editor.populateScriptList();
			Editor.populateLevelList();
		});

		Editor.spriteSheetEditor = SpriteSheetEditor(Editor.assets);
		Editor.spriteSelector = SpriteSelector(Editor.assets, Editor.spriteSheetEditor);
		Editor.animationEditor = AnimationEditor(Editor.assets, Editor.spriteSelector);
		Editor.recipeEditor = RecipeEditor(Editor.assets, Editor.animationEditor);
		Editor.levelEditor = LevelEditor(Editor.assets);

		Editor.initializeVersionCheck();
	},

	initializeVersionCheck: function() {
		$('#editor-version').html('Editor version <span>' + STARDUST_VERSION + '</span>');
		Editor.checkNewVersion();
	},

	checkNewVersion: function() {
		$.getJSON('http://hq.scene.ro/stardust/index.php', function(data) {
			if (data && data.versions && data.versions[0].date > STARDUST_TIMESTAMP) {
				Editor.notifyNewVersion(data.versions[0]);
			} else {
				setTimeout(function() {
					Editor.checkNewVersion();
				}, 1000 * 10);
			}
		});
	},

	notifyNewVersion: function(version) {
		$('#editor-new-version').html("New version available <a href=\"http://hq.scene.ro/stardust/" + version.filename + "\">" + version.version + "</a>");
	},

	createMarkup: function() {
		var content =
		"<ul id=\"recipe-menu\"></ul>" +
		"<div id=\"version-holder\">" +
			"<div id=\"editor-version\"></div>" +
			"<div id=\"editor-new-version\"></div>" +
		"</div>" +
		"<div id=\"level-holder\">" +
			"<div id=\"level\">" +
			"</div>" +
		"</div>" +
		"<div id=\"level-details\">" +
			"<button id=\"btnPreviewLevel\" type=\"button\" class=\"floating-button\" disabled=\"disabled\">Preview</button>" +
			"<button id=\"btnSaveLevel\" type=\"button\" class=\"floating-button\" disabled=\"disabled\">Save</button>" +
			"<div id=\"level-layer-holder\">" +
			"</div>" +
            "<button id=\"btnAddLayer\" type=\"button\" class=\"floating-button\" style=\"clear: left;\" disabled=\"disabled\">Add Layer</button>" +
		"</div>" +
		"<div id=\"editor-tabs\">" +
			"<ul>" + 
				"<li><a href=\"#tabs-recipes\">Recipes</a></li>" + 
				"<li><a href=\"#tabs-animations\">Animations</a></li>" + 
				"<li><a href=\"#tabs-scripts\">Scripts</a></li>" + 
				"<li><a href=\"#tabs-levels\">Levels</a></li>" + 
			"</ul>" + 
			"<div id=\"tabs-recipes\" class=\"tab-holder\">" + 
				"<input type=\"button\" id=\"btnAddRecipe\" value=\"Add Recipe\"/>" +
			"</div>" + 
			"<div id=\"tabs-animations\" class=\"tab-holder\">" + 
				"<input type=\"button\" id=\"btnAddAnimation\" value=\"Add Animation\"/>" +
			"</div>" + 
			"<div id=\"tabs-scripts\" class=\"tab-holder\">" + 
				"<input type=\"button\" id=\"btnAddScript\" value=\"Add Script\"/>" +
			"</div>" + 
			"<div id=\"tabs-levels\" class=\"tab-holder\">" + 
				"<input type=\"button\" id=\"btnAddLevel\" value=\"Add Level\"/>" +
			"</div>" + 
		"</div>"
		;
		$(content).appendTo($('body'));

		$('<ul>')
			.attr('id', 'animation-list')
			.appendTo($('#tabs-animations'));

		$('<ul>')
			.attr('id', 'recipe-list')
			.appendTo($('#tabs-recipes'));

		$('<ul>')
			.attr('id', 'script-list')
			.appendTo($('#tabs-scripts'));

		$('<ul>')
			.attr('id', 'level-list')
			.appendTo($('#tabs-levels'));

		$('#editor-tabs').tabs();
		$(".tab-holder input[type=submit], .tab-holder input[type=button], .tab-holder a, .tab-holder button").button();

		$("button", '#level-details').button();

		$('#btnAddAnimation').click(function() {
			Editor.addAnimation();
		});
		$('#btnAddRecipe').click(function() {
			Editor.addRecipe();
		});
		$('#btnAddScript').click(function() {
			Editor.addScript();
		});
		$('#btnAddLevel').click(function() {
			Editor.addLevel();
		});
	},

	populateAnimationList: function() {
		$('#animation-list').empty();
		for (var i in Editor.assets.animations)
			Editor.addAnimationToList(Editor.assets.animations[i]);
	},

	addAnimationToList: function(animation) {
		var $li = $('<li>');
		$('<a>')
			.text(animation.name)
			.data('animation-name', animation.name)
			.click(function() {
				Editor.editAnimation($(this).data('animation-name'));
			})
			.appendTo($li);

		$li.appendTo($('#animation-list'));
	},

	populateScriptList: function() {
		$('#script-list').empty();
		for (var i in Editor.assets.scripts)
			Editor.addScriptToList(Editor.assets.scripts[i]);
	},

	addScriptToList: function(script) {
		var $li = $('<li>');
		$('<a>')
			.text(script.name)
			.data('script-name', script.name)
			.click(function() {
				Editor.editScript($(this).data('script-name'));
			})
			.appendTo($li);

		$li.appendTo($('#script-list'));
	},

	populateRecipeList: function() {
		$('#recipe-list').empty();
		for (var i in Editor.assets.recipes)
			Editor.addRecipeToList(Editor.assets.recipes[i]);
	},

	addRecipeToList: function(recipe) {
		var $li = $('<li>');
		$('<a>')
			.text(recipe.name)
			.data('recipe-name', recipe.name)
			.click(function() {
				Editor.editRecipe($(this).data('recipe-name'));
			})
			.appendTo($li);

		$li.appendTo($('#recipe-list'));
	},

	populateLevelList: function() {
		$('#level-list').empty();
		for (var i in Editor.assets.levels)
			Editor.addLevelToList(Editor.assets.levels[i]);
	},

	addLevelToList: function(level) {
		var $li = $('<li>');
		$('<a>')
			.text(level.name)
			.data('level-name', level.name)
			.click(function() {
				Editor.editLevel($(this).data('level-name'));
			})
			.appendTo($li);

		$li.appendTo($('#level-list'));
	},

	addAnimation: function() {
		Editor.animationEditor.addAnimation();
	},

	editAnimation: function(name) {
		Editor.animationEditor.editAnimation(name);
	},

	addRecipe: function() {
		Editor.recipeEditor.addRecipe();
	},

	editRecipe: function(name) {
		Editor.recipeEditor.editRecipe(name);
	},

	addScript: function() {
		alert('Not implemented!');
	},

	editScript: function(name) {
		alert('Not implemented!');
	},

	addLevel: function() {
		Editor.levelEditor.addLevel();
	},

	editLevel: function(name) {
		Editor.levelEditor.editLevel(name);
	}
}