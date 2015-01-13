var Editor = {
	assets: null,
	animationEditor: null,
	spriteSelector: null,
	spriteSheetEditor: null,

	initialize: function() {
		Editor.createMarkup();

		Editor.assets = new Assets();
		Editor.assets.initialize(function() {
			console.log('Assets initialized');
			Editor.populateAnimationList();
		});

		Editor.spriteSheetEditor = SpriteSheetEditor(Editor.assets);
		Editor.spriteSelector = SpriteSelector(Editor.assets, Editor.spriteSheetEditor);
		Editor.animationEditor = AnimationEditor(Editor.assets, Editor.spriteSelector);
	},

	createMarkup: function() {
		$('<ul>')
			.attr('id', 'animation-list')
			.appendTo($('body'));
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

		$li
			.appendTo($('#animation-list'));
	},

	populateAnimationList: function() {
		$('#animation-list').empty();
		for (var i in Editor.assets.animations)
			Editor.addAnimationToList(Editor.assets.animations[i]);
	},

	addAnimation: function() {
		Editor.animationEditor.addAnimation();
	},

	editAnimation: function(name) {
		Editor.animationEditor.editAnimation(name);
	}
}