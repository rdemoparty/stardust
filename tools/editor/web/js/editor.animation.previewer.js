	function AnimationPreviewer(assets, canvas) {
		this.assets = assets;
		this.animationName = '';
		this.animation = null;
		this.canvas = canvas;
		this.ctx = this.canvas.getContext('2d');

		this.timer = -1;
		this.animationTime = 0;
		this.animationFrameIndex = 0;
		this.pingPongDirectionRight = true;
	}

	AnimationPreviewer.prototype.preview = function(animationName) {
		this.animationName = animationName;
		this.animation = this.assets.animationByName(animationName);
		this.reset();
	}

	AnimationPreviewer.prototype.reset = function() {
		this.animationTime = 0;
		this.animationFrameIndex = 0;
		this.pingPongDirectionRight = true;
	}

	AnimationPreviewer.prototype.start = function() {
		var self = this;
		this.timer = setInterval(function() {
			self.animate()
		}, 1000 / 30);
	} 

	AnimationPreviewer.prototype.stop = function() {
		clearInterval(this.timer);
	}

	AnimationPreviewer.prototype.animate = function() {
		this.ctx.canvas.width = this.ctx.canvas.width;

		this.ctx.fillStyle = 'rgb(0, 0, 0)';
		this.ctx.fillRect(0, 0, this.ctx.canvas.width, this.ctx.canvas.height);

		if (this.animation == null)
			return;

		this.animationTime += 33; // milliseconds in 30 FPS

		var animationFrameDelay = this.animation.frameTimeInMilliseconds;
		var animationLoopType = this.animation.loopType;

		while (this.animationTime > animationFrameDelay) {
			this.animationTime -= animationFrameDelay;

			switch (animationLoopType) {
				case 'NONE':
					// this.animationFrameIndex++;
					// if (this.animationFrameIndex >= this.animation.frames.length) {
					// 	this.animationFrameIndex = this.animation.frames.length - 1;
					// }
					// break;
				case 'FORWARD':
					this.animationFrameIndex++;
					this.animationFrameIndex %= this.animation.frames.length;
					break;
				case 'REVERSE':
					this.animationFrameIndex--;
					if (this.animationFrameIndex < 0)
						this.animationFrameIndex = this.animation.frames.length - 1;
					break;
				case 'PING_PONG':
					if (this.pingPongDirectionRight)
						this.animationFrameIndex++;
					else
						this.animationFrameIndex--;

					if (this.animationFrameIndex >= this.animation.frames.length) {
						this.animationFrameIndex = this.animation.frames.length - 2; // last frame has already been played
						this.pingPongDirectionRight = !this.pingPongDirectionRight;
					}

					if (this.animationFrameIndex < 0) {
						this.animationFrameIndex = 1; // first frame has already been played
						this.pingPongDirectionRight = !this.pingPongDirectionRight;
					}

					// final index sanitization since the index might be out of rage 
					if (this.animationFrameIndex < 0)
						this.animationFrameIndex = 0;

					if (this.animationFrameIndex >= this.animation.frames.length)
						this.animationFrameIndex = 0;

					break;
				default:
					console.log('Unknown animation loop type ' + animationLoopType);
					break;
			}

		}

		var ssName = this.animation.frames[this.animationFrameIndex].spriteSheet;
		var ssFrame = this.animation.frames[this.animationFrameIndex].index;

		this.drawCenteredSprite(ssName, ssFrame);
	}

	AnimationPreviewer.prototype.drawCenteredSprite = function(ssName, ssFrame) {
		var ss = this.assets.spriteSheetByName(ssName);
		if (ss == null)
			return;

		var frame = ss.frames[ssFrame];

		var centerX = this.ctx.canvas.width / 2;
		var centerY = this.ctx.canvas.height / 2;

		this.ctx.drawImage(
			this.assets.textures[ss.texture], 
			frame.x, frame.y, 
			frame.w, frame.h, 
			centerX - frame.w / 2, centerY - frame.h / 2, 
			frame.w, frame.h
		);
	}