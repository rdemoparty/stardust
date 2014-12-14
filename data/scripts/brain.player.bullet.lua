---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)
    o:setFloat("speed", 1000)
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    local speed = o:getFloat("speed")
    local x, y = o:getPosition()
    y = y - speed * elapsedSeconds
    o:setPosition(x, y)
end

function onDeath(objectPointer, reason)
    local killedEntity = Entity.from(objectPointer)
    if reason == DeathReason.Killed then
        local explosion = Scene.create("enemy.bullet.hit")
        explosion:setPosition(killedEntity:getPosition())
        Scene.add(explosion)
    end
end

