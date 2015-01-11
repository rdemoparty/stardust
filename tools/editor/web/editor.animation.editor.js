function AnimationEditor(assetsInstance, spriteSelectorInstance) {

	var assets = assetsInstance;
	var spriteSelector = spriteSelectorInstance;

	var createMarkup = function() {
		console.log('Creating animation editor markup');
		var $dialog = $('<div>')
							.attr('id', 'dlgAnimation')
							.attr('title', 'Animation Editor')
							.addClass('dialog')
							.hide();

		var dialogContent = "<div id=\"animation-editor-holder\">" +
				"<ul id=\"frames\"></ul>" +
				"<div id=\"animation-editor-sidebar\">" +
					"<div class=\"row\">" +
						"<div class=\"label\">Name</div>" +
						"<div class=\"field\">" +
							"<input type=\"text\" id=\"animation-name\" />" +
						"</div>" +
					"</div>" +
					"<div class=\"row\">" +
						"<div class=\"label\">Loop Type</div>" +
						"<div class=\"field\">" +
							"<select id=\"animation-loop-type\">" +
								"<option value=\"NONE\">NONE</option>" +
								"<option value=\"FORWARD\">FORWARD</option>" +
								"<option value=\"REVERSE\">REVERSE</option>" +
								"<option value=\"PING_PONG\">PING_PONG</option>" +
							"</select>" +
						"</div>" +
					"</div>" +
					"<div class=\"row\">" +
						"<div class=\"label\">Frame delay (ms)</div>" +
						"<div class=\"field\">" +
							"<input type=\"text\" id=\"animation-frame-delay\" />" +
						"</div>" +
					"</div>" +
					"<div id=\"animation-previewer\">" +
						"<canvas id=\"animation-preview-canvas\" width=\"320\" height=\"200\"></canvas>" +
					"</div>" +
				"</div>" +
			"</div>" +
		"</div>";
		$(dialogContent).appendTo($dialog);
		$dialog.appendTo($("body"));

		$('#animation-loop-type').change(function() {
			resetAnimationPlayer();
		});

		$('#animation-frame-delay').keyup(function() {
			resetAnimationPlayer();
		})

		$("#frames").sortable();
		$("#frames").disableSelection();		
	}

	var EDITING_MODE = {
		NONE: 0,
		ADD_NEW: 1,
		EDIT_EXISTING: 2
	};

	var editMode = EDITING_MODE.NONE;
	var originalAnimationName;

	var editAnimation = function(animationName) {
		editMode = EDITING_MODE.EDIT_EXISTING;
		originalAnimationName = animationName;

		var animation = assets.animationByName(animationName);
		$('#animation-name').val(animation.name);
		$('#animation-loop-type').val(animation.loopType);
		$('#animation-frame-delay').val(animation.frameTimeInMilliseconds);

		initializeFrameList(animation);
		openDialog();
	}


	var newAnimation = function() {
		editMode = EDITING_MODE.ADD_NEW;
		// set defaults
		$('#animation-name').val("new_animation");
		$('#animation-loop-type').val('FORWARD');
		$('#animation-frame-delay').val('100');

		initializeFrameList(null);
		openDialog();
	}

	var buildAnimationFromForm = function() {
		var frames = [];
		$('#frames li').each(function() {
			frames[frames.length] = {
				'spriteSheet': $(this).data('spritesheet-name'),
				'index': parseInt($(this).data('spritesheet-frame'))
			}
		})

		return {
			'name': $('#animation-name').val().trim(),
			'frameTimeInMilliseconds': $('#animation-frame-delay').val(),
			'loopType': $('#animation-loop-type').val(),
			'frames': frames
		}
	}

	var isAnimationValid = function(animation) {
		if (animation.name == '') {
			alert('Need to set a name!');
			$('#animation-name').select().focus();
			return false;
		}

		try {
			var delay = parseInt(animation.frameTimeInMilliseconds);
			if (delay < 1 || delay > 3000) {
				alert('Invalid frame delay set!');
				$('#animation-frame-delay').select().focus();
				return false;
			}
			animation.frameTimeInMilliseconds = delay;
		} catch (e) {
			alert('Invalid frame delay set!');
			$('#animation-frame-delay').select().focus();
			return false;
		}

		if (animation.frames.length == 0) {
			alert('No animation frames set!');
			return false;
		}

		return true;
	}

	var saveAnimation = function() {
		var animation = buildAnimationFromForm();
		if (isAnimationValid(animation)) {
			if (editMode == EDITING_MODE.ADD_NEW) {
				if (assets.animationByName(animation.name) != null) {
					alert('An animation with this name already exists!');
					$('#animation-name').select().focus();
					return false;
				}

				assets.addAnimation(animation);
				assets.saveAnimations();
				return true;
			} else {
				var animationNameChanged = animation.name != originalAnimationName;
				if (animationNameChanged) {
					var otherAnim = assets.animationByName(animation.name);
					if (otherAnim !== null) {
						alert('An animation with this name already exists!');
						$('#animation-name').select().focus();
						return false;
					}
					assets.renameAnimation(originalAnimationName, animation.name);
				}
				assets.copyAnimationDataFrom(animation, animation.name);
				assets.saveAnimations();
				return true;
			}
		}

		return false;
	}

	var resetAnimationPlayer = function() {
		animationTime = 0;
		animationFrameIndex = 0;
		pingPongDirectionRight = true;
	}

	var animationTimer;
	var openDialog = function() {
		 $("#dlgAnimation").dialog({
		 	width: '1200',
		 	height: '560',
			modal: true,
			open: function() {
				animationTimer = setInterval(animateEditedAnimation, 1000 / 30); // at 30FPS
				resetAnimationPlayer();
			},
			close: function() {
				clearInterval(animationTimer);
			},
			buttons: {
				'Delete Animation': function() {
					if (editMode == EDITING_MODE.EDIT_EXISTING) {
						if (confirm('Are you sure you want to remove this animation?')) {
							assets.removeAnimation(originalAnimationName);
							assets.saveAnimations();
							Editor.populateAnimationList();
							$(this).dialog('close');
						}
					} else {
						$(this).dialog('close');
					}
				},
				'Delete Selected Frames': function() {
					$('#frames .selected').each(function() {
						$(this).remove();
					})
				},
				'Add Sprite': function() {
					spriteSelector.select(function(spriteSheetName, frameIndex) {
						addAnimationFrameInList(spriteSheetName, frameIndex);
						resetAnimationPlayer();
					});
				},
				'Ok': function() {
					if (saveAnimation()) {
						Editor.populateAnimationList();
						$(this).dialog("close");
					}
				},
				'Cancel': function() {
					$(this).dialog("close");
				}
			}
		});
	}


	var addAnimationFrameInList = function(spriteSheetName, frameIndex) {
		var $li = $('<li>')
						.data('spritesheet-frame', frameIndex)
						.data('spritesheet-name', spriteSheetName)
						.appendTo($('#frames'));

		var spritesheet = assets.spriteSheetByName(spriteSheetName);
		var spriteSheetFrame = spritesheet.frames[frameIndex];

		var $div = $('<div>')
			.data('spritesheet-frame', frameIndex)
			.data('spritesheet-name', spriteSheetName)
			.css('width', spritesheet.spriteWidth + 'px')
			.css('height', spritesheet.spriteHeight + 'px')
			.attr('title', spriteSheetName + '#' + frameIndex)
			.css('background-color', 'rgba(0, 0, 0, 0.7)')
			.css('background-image', 'url(/data/' + spritesheet.texture + ')')
			.css('background-position', '-' + spriteSheetFrame.x + 'px' + ' -' + spriteSheetFrame.y + 'px')
			.click(function() {
				$(this).parent().toggleClass('selected');
			})
			.appendTo($li);
	}

	var initializeFrameList = function(animation) {
		$('#frames').empty();
		if (animation != null)
			for (var i in animation.frames)
				addAnimationFrameInList(animation.frames[i].spriteSheet, animation.frames[i].index);
	}

	var animationTime = 0;
	var animationFrameIndex = 0;
	var pingPongDirectionRight = true;
	function animateEditedAnimation() {
		var canvas = document.getElementById('animation-preview-canvas');
		var ctx = canvas.getContext('2d');

		animationTime += 33; // milliseconds in 30 FPS

		var animationFrameDelay;
		try {
			animationFrameDelay = parseInt($('#animation-frame-delay').val());
			if (animationFrameDelay < 1) {
				animationFrameDelay = 1;
			}
		} catch (e) {
			animationFrameDelay = 1;
		}

		var animationLoopType = $('#animation-loop-type').val();

		while (animationTime > animationFrameDelay) {
			animationTime -= animationFrameDelay;

			switch (animationLoopType) {
				case 'NONE':
					animationFrameIndex++;
					if (animationFrameIndex >= $('#frames li').length) {
						animationFrameIndex = $('#frames li').length - 1;
					}
					break;
				case 'FORWARD':
					animationFrameIndex++;
					animationFrameIndex %= $('#frames li').length;
					break;
				case 'REVERSE':
					animationFrameIndex--;
					if (animationFrameIndex < 0)
						animationFrameIndex = $('#frames li').length - 1;
					break;
				case 'PING_PONG':
					if (pingPongDirectionRight)
						animationFrameIndex++;
					else
						animationFrameIndex--;

					if (animationFrameIndex >= $('#frames li').length) {
						animationFrameIndex = $('#frames li').length - 2; // last frame has already been played
						pingPongDirectionRight = !pingPongDirectionRight;
					}

					if (animationFrameIndex < 0) {
						animationFrameIndex = 1; // first frame has already been played
						pingPongDirectionRight = !pingPongDirectionRight;
					}

					// final index sanitization since the index might be out of rage 
					if (animationFrameIndex < 0)
						animationFrameIndex = 0;

					if (animationFrameIndex >= $('#frames li').length)
						animationFrameIndex = 0;

					break;
				default:
					console.log('Unknown animation loop type ' + animationLoopType);
					break;
			}

		}
		ctx.width = ctx.width;

		ctx.fillStyle = 'rgb(0, 0, 0)';
		ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);

		$li = $('#frames li:eq(' + animationFrameIndex + ')');
		if ($li.length == 0)
			return;

		var ssName = $li.data('spritesheet-name');
		var ssFrame = $li.data('spritesheet-frame');

		drawCenteredSprite(ssName, ssFrame, ctx);
	}

	var drawCenteredSprite = function(ssName, ssFrame, context) {
		var ss = assets.spriteSheetByName(ssName);
		if (ss == null)
			return;

		var frame = ss.frames[ssFrame];

		var centerX = context.canvas.width / 2;
		var centerY = context.canvas.height / 2;

		context.drawImage(
			assets.textures[ss.texture], 
			frame.x, frame.y, 
			frame.w, frame.h, 
			centerX - frame.w / 2, centerY - frame.h / 2, 
			frame.w, frame.h
		);
	}

	// initialize
	createMarkup();

	// expose only what we want
	return {
		editAnimation: editAnimation,
		addAnimation: newAnimation
	}
}