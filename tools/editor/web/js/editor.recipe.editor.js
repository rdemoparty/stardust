var a;

function toInt(value, defaultValue) {
	try {
		var i = parseInt(value);
		return isNaN(i) ? defaultValue : i;
	} catch (e) {
		return defaultValue;
	}
}

function RecipeEditor(assetsInstance, animationEditorInstance) {
	var assets = assetsInstance;
	var animationEditor = animationEditorInstance;
	var animationPreviewer = null;
	var collisionHullEditor = null;
	var editedHull = [];

	var defaultBrainContent = 
"---------------------------------------------------------------------\n" +
"-- The brain functions\n" +
"---------------------------------------------------------------------\n" +
"\n" +
"function onSpawn(objectPointer)\n" +
"    local o = Entity.from(objectPointer)\n" +
"end\n" +
"\n" +
"function onUpdate(objectPointer, elapsedSeconds)\n" +
"    local o = Entity.from(objectPointer)\n" +
"end\n" +
"\n" +
"\n" +
"function onDamage(objectPointer, amount, inflicterPointer)\n" +
"    local o = Entity.from(objectPointer)\n" +
"    local inflicter = Entity.from(inflicterPointer)\n" +
"end\n" +
"\n" +
"\n" +
"function onDeath(objectPointer, reason)\n" +
"    local killedEntity = Entity.from(objectPointer)\n" +
"end";

	var EDITING_MODE = {
		NONE: 0,
		ADD_NEW: 1,
		EDIT_EXISTING: 2
	};

	var editMode = EDITING_MODE.NONE;
	var originalRecipeName;
	var successCallback;

	var addRecipe = function(callback) {
		editMode = EDITING_MODE.ADD_NEW;
		successCallback = callback;

		defaultRecipeToForm();
		openDialog();
	}

	var editRecipe = function(name, callback) {
		editMode = EDITING_MODE.EDIT_EXISTING;
		originalRecipeName = name;
		successCallback = callback;

		var recipe = assets.recipeByName(name);
		if (recipe != null) {
			recipeToForm(recipe);
		}
		openDialog();
	}

	var saveRecipe = function() {
		var recipe = recipeFromForm();
		if (!isRecipeValid(recipe))
			return false;

		if (editMode == EDITING_MODE.ADD_NEW) {
			var existingRecipe = assets.recipeByName(recipe.name);
			if (existingRecipe != null) {
				alert('A recipe with this name already exists');
				$('#recipe-name').select().focus();
				return false;
			}
			assets.addRecipe(recipe);
			var brainContent = $('#recipe-brain-code').data('ace').editor.ace.getSession().getValue();
			assets.saveRecipes(function() {
				Utils.saveContentAsFile(brainContent, recipe.brain, 'text/plain');
			});
			return true;
		} else if (editMode == EDITING_MODE.EDIT_EXISTING) {
			console.log('saving recipe after edit');
			var recipeNameChanged = recipe.name != originalRecipeName;
			if (recipeNameChanged) {
				console.log('recipe name changed');
				var otherRecipe = assets.recipeByName(recipe.name);
				if (otherRecipe !== null) {
					alert('A recipe with this name already exists!');
					$('#recipe-name').select().focus();
					return false;
				}
				assets.renameRecipe(originalRecipeName, recipe.name);
			}
			assets.copyRecipeDataFrom(recipe, recipe.name);

			var brainContent = $('#recipe-brain-code').data('ace').editor.ace.getSession().getValue();
			assets.saveRecipes(function() {
				Utils.saveContentAsFile(brainContent, recipe.brain, 'text/plain');
			});
			return true;
		}
		return false;
	}

	var isRecipeValid = function(recipe) {
		if (recipe.name == '') {
			alert('Need to set a name!');
			$('#recipe-name').select().focus();
			return false;
		}

		if (recipe.animation == '') {
			alert('Need to set an animation!');
			$('#recipe-animation').focus();
			return false;
		}

		if (recipe.brain == '') {
			alert('Need to set brain!');
			$('#recipe-brain').focus();
			return false;
		}

		try {
			var damageOnCollision = parseInt(recipe.damageProvidedOnCollision);
			recipe.damageProvidedOnCollision = damageOnCollision;
		} catch (e) {
			alert('Invalid damage on collision!');
			$('#recipe-damage-on-collision').select().focus();
		}

		try {
			var maxLife = parseInt(recipe.maxLife);
			recipe.maxLife = maxLife;
			if (maxLife < 0) {
				alert('Invalid max life!');
				$('#recipe-max-life').select().focus();
			}
		} catch (e) {
			alert('Invalid max life!');
			$('#recipe-max-life').select().focus();
		}

		return true;
	}

	var previewEntity = function() {
		assets.stashRecipes();

		var recipe = recipeFromForm();
		recipe.name = '__preview_entity';
		recipe.brain = 'scripts/__preview_brain.lua';
	
		var brainContent = $('#recipe-brain-code').data('ace').editor.ace.getSession().getValue();
		Utils.saveContentAsFile(brainContent, recipe.brain, 'text/plain', function() {
			assets.addRecipe(recipe);
			assets.saveRecipes(function() {
				$.getJSON('/editor/preview-entity/' + recipe.name, function(data) {
					assets.restoreRecipes();
				});
			});
		});
	}

	var showCodeInEditor = function(code) {		
 		$('#recipe-brain-code').ace({ 
 			theme: 'monokai', 
 			lang: 'lua' 
 		});
		$('#recipe-brain-code').data('ace').editor.ace.getSession().setValue(code);
	}

	var showBrainInEditor = function(brain) {
		$.get("/data/" + brain, function(data) {
			showCodeInEditor(data);
		}, 'text');
	}

	var defaultRecipeToForm = function() {
		fillInAnimations('');
		fillInScripts('');
		$('#recipe-name').val("unnamed");
		$('#recipe-damage-on-collision').val('');
		$('#recipe-collidable').attr('checked', true);
		$('#recipe-remove-on-death').attr('checked', true);
		$('#recipe-kill-if-outside').attr('checked', true);
		$('#recipe-max-life').val('');
		$('#recipe-team').val('');
		$('#recipe-type').val('');

		$('#recipe-hull').val(JSON.stringify([], null, 2));

		$('#dlgRecipeEditor .flags').buttonset('refresh');

		// apparently the first call to creating the editor needs either the form to be visible
		// or some delay.
		setTimeout(function() {
			showCodeInEditor(defaultBrainContent);
		}, 2);
	}

	var recipeToForm = function(recipe) {
		fillInAnimations(recipe.animation);
		fillInScripts(recipe.brain);
		$('#recipe-name').val(recipe.name);
		$('#recipe-damage-on-collision').val(recipe.damageProvidedOnCollision);
		$('#recipe-collidable').attr('checked', recipe.collidable);
		$('#recipe-remove-on-death').attr('checked', recipe.removeOnDeath);
		$('#recipe-kill-if-outside').attr('checked', recipe.killIfOutside);
		$('#recipe-max-life').val(recipe.maxLife);
		$('#recipe-team').val(recipe.team);
		$('#recipe-type').val(recipe.type);

		if ('hull' in recipe)
			$('#recipe-hull').val(JSON.stringify(recipe.hull, null, 2));
		else
			$('#recipe-hull').val(JSON.stringify([], null, 2));

		$('#dlgRecipeEditor .flags').buttonset('refresh');

		showBrainInEditor(recipe.brain);
	}

	var recipeFromForm = function() {
		return {
			'name': $('#recipe-name').val().trim(),
			'animation': $('#recipe-animation').val(),
			'brain': $('#recipe-brain').val(),
			'damageProvidedOnCollision': toInt($('#recipe-damage-on-collision').val(), 0),
			'collidable': $('#recipe-collidable').is(':checked'),
			'removeOnDeath': $('#recipe-remove-on-death').is(':checked'),
			'killIfOutside': $('#recipe-kill-if-outside').is(':checked'),
			'maxLife': toInt($('#recipe-max-life').val(), 0),
			'team': $('#recipe-team').val(),
			'type': $('#recipe-type').val(),
			'hull': JSON.parse($('#recipe-hull').val())
		};
	}

	var openDialog = function() {
		$("#dlgRecipeEditor").dialog({
			width: '1200',
			height: '610',
			modal: true,
			open: function() {				
				animationPreviewer.start();
				animationPreviewer.preview($('#recipe-animation').val());

				editedHull = JSON.parse($('#recipe-hull').val());
				collisionHullEditor.start(editedHull, $('#recipe-animation').val());
			},
			close: function() {
				animationPreviewer.stop();
				collisionHullEditor.stop();
			},
			buttons: {
				'Preview': function() {
					previewEntity();
				},
				'Ok': function() {
					$('#recipe-hull').val(JSON.stringify(editedHull, null, 2));
					if (saveRecipe()) {
						// TODO: this should go in the success callback, lowering the coupling
						Editor.populateRecipeList();
						$(this).dialog("close");
						if (typeof successCallback != 'undefined') {
							successCallback($('#recipe-name').val());
						}
					}
				},
				'Cancel': function() {
					$(this).dialog("close");
				}
			}
		});
	}

	var fillInAnimations = function(selectedAnimName) {
		var $list = $('#recipe-animation').empty();
		$('<option>')
			.val("")
			.text("")
			.appendTo($list);	
		$('<option>')
			.val("_new_anim_")
			.text("--- New Animation ---")
			.appendTo($list);	
		for (var i in assets.animations) {
			$('<option>')
				.val(assets.animations[i].name)
				.text(assets.animations[i].name)
				.appendTo($list);				
		}

		if (typeof selectedAnimName != 'undefined')
			$list.val(selectedAnimName);
	}

	var fillInScripts = function(selectedBrainName) {
		var $list = $('#recipe-brain').empty();
		$('<option>')
			.val("")
			.text("")
			.appendTo($list);	
		$('<option>')
			.val("_new_brain_")
			.text("--- New Brain ---")
			.appendTo($list);	
		for (var i in assets.scripts) {
			$('<option>')
				.val(assets.scripts[i].uri)
				.text(assets.scripts[i].name)
				.appendTo($list);				
		}

		if (typeof selectedBrainName != 'undefined')
			$list.val(selectedBrainName);
	}

	var createMarkup = function() {
		console.log('Creating recipe editor markup');
		var $dialog = $('<div>')
							.attr('id', 'dlgRecipeEditor')
							.attr('title', 'Recipe Editor')
							.addClass('dialog')
							.hide();

		var dialogContent = 
			"<div id=\"recipe-editor-holder\">" +
				"<div id=\"recipe-editor-sidebar\">" + 
					"<div class=\"row\">" +
						"<div class=\"flags\">" +
							"<input type=\"checkbox\" id=\"recipe-collidable\" /><label for=\"recipe-collidable\">Collidable</label>" +
							"<input type=\"checkbox\" id=\"recipe-remove-on-death\" /><label for=\"recipe-remove-on-death\">Remove on death</label>" +
							"<input type=\"checkbox\" id=\"recipe-kill-if-outside\" /><label for=\"recipe-kill-if-outside\">Kill if outside</label>" +
						"</div>" +
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Name</div>" + 
						"<div class=\"field\">" + 
							"<input type=\"text\" id=\"recipe-name\" value=\"\" />" +
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Animation</div>" + 
						"<div class=\"field\">" + 
							"<select id=\"recipe-animation\"></select>" + 
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Brain</div>" + 
						"<div class=\"field\">" + 
							"<select id=\"recipe-brain\"></select>" + 
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Damage provided on collision</div>" + 
						"<div class=\"field\">" + 
							"<input type=\"text\" id=\"recipe-damage-on-collision\" value=\"\" />" +
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Maximum life</div>" + 
						"<div class=\"field\">" + 
							"<input type=\"text\" id=\"recipe-max-life\" value=\"\" />" +
						"</div>" + 
					"</div>" + 
					// this one holds the JSON representation of the collision hull
					"<input type=\"hidden\" id=\"recipe-hull\" value=\"\" />" +
					"<div class=\"row\">" +
						"<div class=\"label\">Team</div>" + 
						"<div class=\"field\">" + 
							"<select id=\"recipe-team\">" +
								"<option value=\"Friendly\">Friendly</option>" + 
								"<option value=\"Adverse\">Adverse</option>" + 
								"<option value=\"Neutral\">Neutral</option>" + 
							"</select>" +
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Type</div>" + 
						"<div class=\"field\">" + 
							"<select id=\"recipe-type\">" +
								"<option value=\"Scenery\">Scenery</option>" + 
								"<option value=\"Ship\">Ship</option>" + 
								"<option value=\"Bullet\">Bullet</option>" + 
								"<option value=\"Explosion\">Explosion</option>" + 
								"<option value=\"Bonus\">Bonus</option>" + 
							"</select>" +
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<canvas id=\"recipe-animation-preview-canvas\" width=\"320\" height=\"200\"></canvas>" +
					"</div>" +
				"</div>" +
				"<div id=\"recipe-work-area\">" + 
					"<div id=\"recipe-tabs\">" +
						"<ul>" + 
							"<li><a href=\"#recipe-tab-brain\">Brain</a></li>" + 
							"<li><a href=\"#recipe-tab-collision-hull\">Collision Hull</a></li>" + 
						"</ul>" + 
						"<div id=\"recipe-tab-collision-hull\" class=\"tab-holder\">" + 
							"<canvas id=\"recipe-collision-hull-canvas\" width=\"480\" height=\"400\"></canvas>" +
							"<div id=\"recipe-collision-hull-items\">" + 
							"</div>" + 
						"</div>" + 
						"<div id=\"recipe-tab-brain\" class=\"tab-holder\">" + 
							"<textarea id=\"recipe-brain-code\" rows=\"20\"></textarea>"
						"</div>" + 
					"</div>"
				"</div>" + 
			"</div>";
		$(dialogContent).appendTo($dialog);
		$dialog.appendTo($("body"));

		fillInAnimations('');
		fillInScripts('');

		$('#recipe-tabs').tabs();
		$('.flags', $dialog).buttonset();

		$('#recipe-animation').change(function() {
			if ($(this).val() == '_new_anim_') {
				animationEditor.addAnimation(function(newAnimName) {
					fillInAnimations(newAnimName);
				})
				$(this).val(""); // if we escape from animation editor without saving, we want to reset state
			} else {
				console.log("Selected animation " + $(this).val());
				animationPreviewer.preview($(this).val());
				collisionHullEditor.start(editedHull, $(this).val());
			}
		});

		$('#recipe-brain').change(function() {
			if ($(this).val() == '_new_brain_') {
				brainEditor.addBrain(function(newBrainName) {
					fillInScripts(newBrainName);
				})
				$(this).val(""); // if we escape from brain editor without saving, we want to reset state
			} else {
				console.log("Selected brain " + $(this).val());
				showBrainInEditor($(this).val());
			}
		});

		animationPreviewer = new AnimationPreviewer(assets, document.getElementById('recipe-animation-preview-canvas')); 
		collisionHullEditor = new CollisionHullEditor(
			assets, 
			document.getElementById('recipe-collision-hull-canvas'), 
			document.getElementById('recipe-collision-hull-items')
		);
	}

	createMarkup();

	return {
		addRecipe: addRecipe,
		editRecipe: editRecipe
	}
}