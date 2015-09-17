---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    Scene.pauseLevel()
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)
    
    local x, y = o:getPosition()
    y = y + 100 * elapsedSeconds
    o:setPosition(x, y)
end


function onDeath(objectPointer, reason)
    Scene.resumeLevel()
end

