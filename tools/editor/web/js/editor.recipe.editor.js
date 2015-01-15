function RecipeEditor(assetsInstance) {
	var assets = assetsInstance;

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

	var defaultRecipeToForm = function() {
		$('#recipe-name').val("unnamed");
		$('#recipe-animation').val('');
		$('#recipe-brain').val('');
		$('#recipe-damage-on-collision').val('');
		$('#recipe-collidable').attr('checked', false);
		$('#recipe-remove-on-death').attr('checked', false);
		$('#recipe-kill-if-outside').attr('checked', false);
		$('#recipe-max-life').val('');
		$('#recipe-team').val('');
		$('#recipe-type').val('');

		$('#dlgRecipeEditor .flags').buttonset('refresh');
 		$('#recipe-brain-code').ace({ 
 			theme: 'monokai', 
 			lang: 'lua' 
 		});
		$('#recipe-brain-code').data('ace').editor.ace.getSession().setValue(defaultBrainContent);
	}

	var recipeToForm = function(recipe) {
		$('#recipe-name').val(recipe.name);
		$('#recipe-animation').val(recipe.animation);
		$('#recipe-brain').val(recipe.brain);
		$('#recipe-damage-on-collision').val(recipe.damageProvidedOnCollision);
		$('#recipe-collidable').attr('checked', recipe.collidable);
		$('#recipe-remove-on-death').attr('checked', recipe.removeOnDeath);
		$('#recipe-kill-if-outside').attr('checked', recipe.killIfOutside);
		$('#recipe-max-life').val(recipe.maxLife);
		$('#recipe-team').val(recipe.team);
		$('#recipe-type').val(recipe.type);

		$('#dlgRecipeEditor .flags').buttonset('refresh');

		$.get("/data/" + recipe.brain, function(data) {
	 		$('#recipe-brain-code').ace({ 
	 			theme: 'monokai', 
	 			lang: 'lua' 
	 		});
			$('#recipe-brain-code').data('ace').editor.ace.getSession().setValue(data);
		}, 'text');
	}

	var openDialog = function() {
		$("#dlgRecipeEditor").dialog({
			width: '1200',
			height: '560',
			modal: true,
			open: function() {
			},
			close: function() {
			},
			buttons: {
				'Ok': function() {
					$(this).dialog("close");
				},
				'Cancel': function() {
					$(this).dialog("close");
				}
			}
		});
	}

	var brainEditor;
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
							"<input type=\"text\" id=\"recipe-animation\" value=\"\" />" +
						"</div>" + 
					"</div>" + 
					"<div class=\"row\">" +
						"<div class=\"label\">Brain</div>" + 
						"<div class=\"field\">" + 
							"<input type=\"text\" id=\"recipe-brain\" value=\"\" />" +
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
				"</div>" +
				"<div id=\"recipe-work-area\">" + 
					"<div id=\"recipe-tabs\">" +
						"<ul>" + 
							"<li><a href=\"#recipe-tab-brain\">Brain</a></li>" + 
							"<li><a href=\"#recipe-tab-collision-hull\">Collision Hull</a></li>" + 
						"</ul>" + 
						"<div id=\"recipe-tab-brain\" class=\"tab-holder\">" + 
							"<textarea id=\"recipe-brain-code\" rows=\"20\">" + defaultBrainContent + "</textarea>"
						"</div>" + 
						"<div id=\"recipe-tab-collision-hull\" class=\"tab-holder\">" + 
						"</div>" + 
					"</div>"
				"</div>" + 
			"</div>";
		$(dialogContent).appendTo($dialog);
		$dialog.appendTo($("body"));

		$('#recipe-tabs').tabs();
		$('.flags', $dialog).buttonset();
	}

	createMarkup();

	return {
		addRecipe: addRecipe,
		editRecipe: editRecipe
	}
}