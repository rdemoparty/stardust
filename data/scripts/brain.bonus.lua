---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)
    o:setFloat("phase", 0)
    o:setFloat("speed", 1.5)
    local x, y = o:getPosition()
    o:setFloat("y", y)
    o:setScale(0.65)

end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    local rotation = o:getRotation() + elapsedSeconds
    o:setRotation(rotation)

    local phase = o:getFloat("phase") + o:getFloat("speed") * math.pi * elapsedSeconds
    o:setFloat("phase", phase)

    local x, y = o:getPosition()
    y = o:getFloat("y") + math.sin(phase) * 10
    o:setPosition(x, y)

    -- if o:isAnimationFinished() then
    --     o:kill(DeathReason.JobFinished)
    -- end
end


-- We have been touched by player
function onDamage(objectPointer, amount, inflicterPointer)
    local o = Entity.from(objectPointer)
    local player = Entity.from(inflicterPointer)

    if math.random(10) > 5 then
        playSong("menu3.ogg")
    else
        playSong("main.ogg")
    end

    local playerLife = player:getLife() + 10
    if playerLife > player:getMaxLife() then
    	playerLife = player:getMaxLife()
    end
    player:setLife(playerLife)
end


function onDeath(objectPointer, reason)
    local killedEntity = Entity.from(objectPointer)

    local beacon = Scene.create("beacon")
    beacon:setPosition(killedEntity:getPosition())
    Scene.add(beacon)
end

