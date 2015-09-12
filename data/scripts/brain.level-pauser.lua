---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)
    Scene.pauseLevel()
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)
    
    local x, y = o:getPosition()
    y = y + 100 * elapsedSeconds
    o:setPosition(x, y)
end


-- We have been touched by player
function onDamage(objectPointer, amount, inflicterPointer)
    local o = Entity.from(objectPointer)
end


function onDeath(objectPointer, reason)
    Scene.resumeLevel()
end

