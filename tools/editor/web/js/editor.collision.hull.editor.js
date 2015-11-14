function EditorViewState(canvas) {
	this.reset();
	this.canvas = canvas;
	this.context = this.canvas.getContext('2d');
}

EditorViewState.prototype.reset = function() {
	this.mousePressed = false;

	// panning
	this.offsetX = 0;
	this.offsetY = 0;

	// drag start point
	this.dragStartX = 0;
	this.dragStartY = 0;

	// current drag amount
	this.dragX = 0;
	this.dragY = 0;

	this.moveStartX = 0;
	this.moveStartY = 0;

	this.moveX = 0;
	this.moveY = 0;

	this.isPanning = false;
	this.selectedHull = null;

	this.scale = 1.0;
}

EditorViewState.prototype.applyTransform = function() {
	this.context.save();

	var s = this.scale;
	this.context.translate(this.context.canvas.width / 2, this.context.canvas.height / 2);
	this.context.scale(s, s);
	this.context.translate(
		this.offsetX + (this.dragX - this.dragStartX) / s, 
		this.offsetY + (this.dragY - this.dragStartY) / s
	);
}

EditorViewState.prototype.startPanningAt = function(x, y) {
	this.mousePressed = true;
	this.isPanning = true;
	this.selectedHull = null;
	this.dragStartX = x;
	this.dragStartY = y;
	this.dragX = this.dragStartX;
	this.dragY = this.dragStartY;
}

EditorViewState.prototype.startMovingHullAt = function(hull, x, y) {
	this.mousePressed = true;
	this.selectedHull = hull;
	this.moveStartX = x;
	this.moveStartY = y;
	this.moveX = this.moveStartX;
	this.moveY = this.moveStartY;
}

EditorViewState.prototype.getCurrentHullMoveAmount = function() {
	if (!this.mousePressed)
		return {x: 0, y: 0};

	var s = this.scale;
	return {
		'x': (this.moveX - this.moveStartX) / s,
		'y': (this.moveY - this.moveStartY) / s
	}
}

EditorViewState.prototype.stopDragging = function() {
	if (this.isPanning || this.selectedHull != null) {
		var s = this.scale;
		if (this.isPanning) {
			this.offsetX += (this.dragX - this.dragStartX) / s;
			this.offsetY += (this.dragY - this.dragStartY) / s;
		} else {
			this.selectedHull.x += (this.moveX - this.moveStartX) / s;
			this.selectedHull.y += (this.moveY - this.moveStartY) / s;
		}
		this.isPanning = false;
		this.mousePressed = false;
		this.dragStartX = this.dragStartY = this.dragX = this.dragY = 0;
		this.moveStartX = this.moveStartY = this.moveX = this.moveY = 0;
	}
}

EditorViewState.prototype.moveMouseTo = function(x, y) {
	if (this.isPanning) {
		this.dragX = x;
		this.dragY = y;
	} else if (this.selectedHull != null && this.mousePressed) {
		this.moveX = x;
		this.moveY = y;
	}
}

EditorViewState.prototype.zoomBy = function(amount) {
	if (this.selectedHull != null) {
		var newRadius = this.selectedHull.radius + amount*10;
		if (newRadius > 0.1) {
			this.selectedHull.radius = newRadius;
		}
	} else {
		var newScale = this.scale + amount*4;
		if (newScale > 0.1) {
			this.scale = newScale;
		}
	}
}

EditorViewState.prototype.restoreTransform = function() {
	this.context.restore();
}

function CollisionHullEditor(assets, canvas, itemsHolder) {
	this.assets = assets;
	this.canvas = canvas;
	this.itemsHolder = itemsHolder;
	this.ctx = this.canvas.getContext('2d');
	this.timer = -1;
	this.animation = null;
	this.hull = null;

	this.viewTransform = new EditorViewState(this.canvas);
	this.selectedHullPart = null;

	// using composition to reuse animation drawing capabilities of the animation previewer
	this.animationPreviewer = new AnimationPreviewer(assets, canvas);
	this.animationPreviewer.centerAround(0, 0);

	// hook up events
	var self = this;
	$(this.canvas)
		.mousedown(function(e) {
			self.selectedHullPart = null;

			if (self.hull != null) {
				for (var i in self.hull) {
					if (self.isPositionInsideHullPart(e.offsetX, e.offsetY, self.hull[i])) {
						self.selectedHullPart = self.hull[i];
						break;
					}
				}
			}

			if (self.selectedHullPart == null) {
				self.viewTransform.startPanningAt(e.offsetX, e.offsetY);
				$(this).css('cursor', 'pointer');
			} else {
				self.viewTransform.startMovingHullAt(self.selectedHullPart, e.offsetX, e.offsetY);
			}
			self.constructHullItemsTable();
			return false;
		})
		.mouseup(function(e) {
			self.viewTransform.stopDragging();
			self.constructHullItemsTable();
			$(this).css('cursor', '');
		})
		.dblclick(function(e) {
			// TODO: extract this in EditorViewTransform 
			var s = self.viewTransform.scale;
			var cartesianX = (e.offsetX - self.canvas.width / 2) / s - self.viewTransform.offsetX;
			var cartesianY = (e.offsetY - self.canvas.height / 2) / s - self.viewTransform.offsetY;

			self.hull[self.hull.length] = {
				'x': cartesianX,
				'y': cartesianY,
				'radius': 10
			}
			self.selectHullIndex(self.hull.length - 1);
			self.constructHullItemsTable();
			return false;
		})
		.mousemove(function(e) {
			self.viewTransform.moveMouseTo(e.offsetX, e.offsetY);
			return false;
		})
		.mousewheel(function(e) {
			self.viewTransform.zoomBy((e.deltaY * e.deltaFactor) / 300.0);
			self.constructHullItemsTable();
			return false;
		})

	$('#recipe-work-area').keydown(function(e) {
		// check if delete has been pressed
		if (e.which == 46) {
 			if (self.selectedHullPart != null) {
				self.removeSelectedHullPart();
				e.preventDefault();
				return false;
 			}
		}
	});	

	$(this.itemsHolder).on('click', '.hull-item-row', function() {
		var hullIndex = $(this).data('hullIndex');
		self.selectHullIndex(hullIndex);
	});
}

CollisionHullEditor.prototype.removeSelectedHullPart = function() {
	var hullIndex = $('.hull-item-row + .selected').data('hullIndex');
	this.selectedHullPart = null;
	this.hull.splice(hullIndex, 1);
	this.constructHullItemsTable();
}

CollisionHullEditor.prototype.selectHullIndex = function(index) {
	this.selectedHullPart = this.hull[index];
	this.constructHullItemsTable();
}

CollisionHullEditor.prototype.isPositionInsideHullPart = function(x, y, hullPart) {
	// TODO: extract this to viewTransform
	var s = this.viewTransform.scale;
	var cartesianX = (x - this.canvas.width / 2) / s - this.viewTransform.offsetX - hullPart.x;
	var cartesianY = (y - this.canvas.height / 2) / s - this.viewTransform.offsetY - hullPart.y;

	if (Math.sqrt(cartesianX*cartesianX + cartesianY*cartesianY) <= hullPart.radius) {
		return true;
	} else {
		return false;
	}
}

CollisionHullEditor.prototype.start = function(hull, animationName) {
	this.hull = hull;
	this.animation = this.assets.animationByName(animationName);
	this.animationPreviewer.preview(animationName);

	if (this.timer == -1) {
		var self = this;
		this.timer = setInterval(function() {
			self.render()
		}, 1000 / 10);
	}

	this.constructHullItemsTable();
}


CollisionHullEditor.prototype.render = function() {
	this.ctx.canvas.width = this.ctx.canvas.width;

	this.ctx.fillStyle = 'rgb(33, 33, 33)';
	this.ctx.fillRect(0, 0, this.ctx.canvas.width, this.ctx.canvas.height);

	this.viewTransform.applyTransform(this.ctx);

	this.animationPreviewer.animate();

	if (this.hull != null) 
		for (var i in this.hull)
			this.renderHullPart(this.hull[i]);

	this.viewTransform.restoreTransform(this.ctx);
}

CollisionHullEditor.prototype.renderHullPart = function(hullPart) {
	var offset = {
		'x': 0,
		'y': 0
	}

	if (hullPart == this.selectedHullPart) {
		this.ctx.strokeStyle = 'rgba(255, 0, 0, 0.7)';
		offset = this.viewTransform.getCurrentHullMoveAmount();
	} else {
		this.ctx.strokeStyle = 'rgba(255, 255, 255, 0.7)';
	}
	this.ctx.lineWidth = 1.6;
	this.ctx.beginPath();
	this.ctx.arc(hullPart.x + offset.x, hullPart.y + offset.y, hullPart.radius, 0, 2*Math.PI);
	this.ctx.stroke();
}

CollisionHullEditor.prototype.stop = function() {
	clearInterval(this.timer);
	this.timer = -1;
}

CollisionHullEditor.prototype.constructHullItemsTable = function() {
	$(this.itemsHolder).empty();
	if (this.hull != null)
		for (var i in this.hull)
			this.addHullItemToTable(i);
}


CollisionHullEditor.prototype.addHullItemToTable = function(i) {
	var hull = this.hull[i];

	$row = $('<div>')
				.attr('id', 'hull-item-row' + i)
				.data('hullIndex', i)
				.addClass('hull-item-row')
				.append($('<div>')
							.addClass('hull-item-cell')
							.addClass('hull-item-x')
							.text(this.formatFloat(hull.x)))
				.append($('<div>')
							.addClass('hull-item-cell')
							.addClass('hull-item-y')
							.text(this.formatFloat(hull.y)))
				.append($('<div>')
							.addClass('hull-item-cell')
							.addClass('hull-item-radius')
							.text(this.formatFloat(hull.radius)))
				.appendTo($(this.itemsHolder));

	if (hull == this.selectedHullPart)
		$row.addClass('selected');
}

CollisionHullEditor.prototype.formatFloat = function(val) {
	var result = Math.round(val * 100) / 100;
	return result;
}

