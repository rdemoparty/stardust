var a;

function RecipeEditor(assetsInstance, animationEditorInstance) {
	var assets = assetsInstance;
	var animationEditor = animationEditorInstance;
	var animationPreviewer = null;

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

	var addRecipe = function() {
		defaultRecipeToForm();
		openDialog();
	}

	var editRecipe = function(name) {
		var recipe = assets.recipeByName(name);
		if (recipe != null) {
			recipeToForm(recipe);
		}
		openDialog();
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

		$('#dlgRecipeEditor .flags').buttonset('refresh');

		showBrainInEditor(recipe.brain);
	}

	var recipeFromForm = function() {
		return {
			'name': $('#recipe-name').val().trim(),
			'animation': $('#recipe-animation').val(),
			'brain': $('#recipe-brain').val(),
			'damageProvidedOnCollision': $('#recipe-damage-on-collision').val(),
			'collidable': $('#recipe-collidable').is(':checked'),
			'removeOnDeath': $('#recipe-remove-on-death').is(':checked'),
			'killIfOutside': $('#recipe-kill-if-outside').is(':checked'),
			'maxLife': $('#recipe-max-life').val(),
			'team': $('#recipe-team').val(),
			'type': $('#recipe-type').val()
		};
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

	var openDialog = function() {
		$("#dlgRecipeEditor").dialog({
			width: '1200',
			height: '560',
			modal: true,
			open: function() {				
				animationPreviewer.start();
				animationPreviewer.preview($('#recipe-animation').val());
			},
			close: function() {
				animationPreviewer.stop();
			},
			buttons: {
				'Preview': function() {
					previewEntity();
				},
				'Ok': function() {
					$(this).dialog("close");
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
						"<div id=\"recipe-tab-brain\" class=\"tab-holder\">" + 
							"<textarea id=\"recipe-brain-code\" rows=\"20\"></textarea>"
						"</div>" + 
						"<div id=\"recipe-tab-collision-hull\" class=\"tab-holder\">" + 
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
	}

	createMarkup();

	return {
		addRecipe: addRecipe,
		editRecipe: editRecipe
	}
}