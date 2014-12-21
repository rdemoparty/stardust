function easeIn(time, startValue, changeInValue, duration)
    t = time / duration
    return changeInValue * t * t * t * t * t + startValue 
end

function easeOut(time, startValue, changeInValue, duration)
    t = time / duration - 1
    return changeInValue * (t*t*t*t*t + 1) + startValue 
end

---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    -- update position
    local x, y = o:getPosition()
    local t = o:getFloat("elapsedSeconds") + elapsedSeconds
    o:setFloat("elapsedSeconds", t)

    y = easeIn(t, 800, -260, 2)

    o:setPosition(x, y)

    -- persist position and velocity
    if t >= 2 then
        o:setBrain("scripts/brain.player.lua")
    end
end

function onDeath(objectPointer, reason)
end