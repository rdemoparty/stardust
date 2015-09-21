---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    Level.pause()
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)
    
    local x, y = o:getPosition()
    y = y + 100 * elapsedSeconds
    o:setPosition(x, y)
end


function onDeath(objectPointer, reason)
    Level.resume()
end

