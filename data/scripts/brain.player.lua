---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

SQRT2 = math.sqrt(2)

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)
    o:setFloat("speed", 300)
end

function velocityFromInput()
    local velocityX = 0
    local velocityY = 0

    if leftPressed() then
    	velocityX = -1
    elseif rightPressed() then
    	velocityX = 1
    end

    if upPressed() then
    	velocityY = -1
    elseif downPressed() then
    	velocityY = 1
    end

    if (math.abs(velocityX) > 0.00001) and (math.abs(velocityY) > 0.00001) then
    	velocityX = velocityX / SQRT2
    	velocityY = velocityY / SQRT2
    end

    return velocityX, velocityY
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    if fireJustPressed() then
    	o:fire(true)
    elseif fireJustReleased() then
    	o:fire(false)
    end

    local speed = o:getFloat("speed")

    local vx, vy = velocityFromInput()
    local x, y = o:getPosition()

    x = x + vx * speed * elapsedSeconds
    y = y + vy * speed * elapsedSeconds

    o:setPosition(x, y)
    Scene.confineToPlayingArea(o)
end


function onDeath(objectPointer, reason)
end

