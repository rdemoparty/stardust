function SpriteSelector(assetsInstance, spriteSheetEditorInstance) {

	var assets = assetsInstance;
	var ssEditor = spriteSheetEditorInstance;

	var initializeSpriteList = function(filter) {
		$('#sprites').empty();

		for (var i in assets.spritesheets) {
			var spritesheet = assets.spritesheets[i];
			if (filter == '' || filter == spritesheet.name) {
				for (var j in spritesheet.frames) {
					var $li = $('<li>')
									.data('spritesheet-frame', j)
									.data('spritesheet-name', spritesheet.name)
									.appendTo($('#sprites'));

					var spriteSheetFrame = spritesheet.frames[j];

					var $div = $('<div>')
						.data('spritesheet-frame', j)
						.data('spritesheet-name', spritesheet.name)
						.css('width', spritesheet.spriteWidth + 'px')
						.css('height', spritesheet.spriteHeight + 'px')
						.attr('title', spritesheet.nam + '#' + j)
						.css('background-color', 'rgba(0, 0, 0, 0.7)')
						.css('background-image', 'url(/data/' + spritesheet.texture + ')')
						.css('background-position', '-' + spriteSheetFrame.x + 'px' + ' -' + spriteSheetFrame.y + 'px')
						.click(function() {
							$(this).parent().toggleClass('selected');
						})
						.appendTo($li);
				}
			}
		}
	}

	var rebuildFilter = function() {
		$filter = $('#spriteSheetFilter').empty();
		$('<option>').text('All').val('').appendTo($filter);

		for (var i in assets.spritesheets)
			$('<option>')
				.text(assets.spritesheets[i].name)
				.val(assets.spritesheets[i].name)
				.appendTo($filter);
	}

	var selectSprite = function(spriteSelectedCallback) {
		rebuildFilter();
		initializeSpriteList('');

		$("#dlgSelectSprite").dialog({
			width: '1200',
			height: '560',
			modal: true,
			buttons: {
				'New SpriteSheet': function() {
					ssEditor.new(function(ss) {
						assets.preloadTexture(ss.texture, false, function(loadedTextureName) {
							assets.calculateSpritesheetFrames();
							rebuildFilter();
							$('#spriteSheetFilter').val(ss.name);
							initializeSpriteList(ss.name);
						});
					});
				},
				'Ok': function() {
					$('#sprites .selected').each(function() {
						spriteSelectedCallback(
							$(this).data('spritesheet-name'),
							$(this).data('spritesheet-frame')
						);
					});
					$(this).dialog("close");
				}
			}
		});
	}

	var createMarkup = function() {
		console.log('Creating sprite selector markup');
		var $dialog = $('<div>')
						.attr('id', 'dlgSelectSprite')
						.attr('title', 'Sprite Selector')
						.addClass('dialog')
						.hide();
		var dialogContent = "<div style=\"margin-bottom: 1em;\">" +
				"Filter <select id=\"spriteSheetFilter\"></select>" +
			"</div>" +
			"<ul id=\"sprites\"></ul>";
			
		$(dialogContent).appendTo($dialog);
		$dialog.appendTo($("body"));

		$('#spriteSheetFilter').change(function() {
			initializeSpriteList($(this).val());
		});

	}

	createMarkup();

	return {
		'select': selectSprite,
		'rebuildFilter': rebuildFilter,
		'setFilter': initializeSpriteList
	}
}
