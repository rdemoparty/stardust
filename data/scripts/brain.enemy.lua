---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)

    x, y = o:getPosition()

    o:setFloat("center", x)
    o:setFloat("speed", 200)
    o:setFloat("aperture", math.random(100, 400))
    o:setFloat("phase", math.random() * math.pi * 2.0)

    o:fire(true)
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    local phase = o:getFloat("phase") + elapsedSeconds
    o:setFloat("phase", phase)

    -- o:setRotation(o:getRotation() + 5.0 * elapsedSeconds)
    -- o:setColor(1, 1, 1, 0.3 * math.sin(phase*2) + 0.6)
    -- o:setScale(0.6 * math.sin(phase*5) + 1)

    local speed = o:getFloat("speed")
    local x, y = o:getPosition()
    x = o:getFloat("aperture") * math.sin(phase) + o:getFloat("center")
    y = y + speed * elapsedSeconds
    o:setPosition(x, y)
end

function onDeath(objectPointer, reason)
    local killedEntity = Entity.from(objectPointer)
    if reason == DeathReason.Killed then
        local explosion = Scene.create("enemy.death.explosion")
        explosion:setPosition(killedEntity:getPosition())
        Scene.add(explosion)
    end
end

