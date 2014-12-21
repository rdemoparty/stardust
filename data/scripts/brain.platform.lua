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

    o:setInt("state", 0)    
    o:setFloat("maxSpeed", 400)    
    o:setFloat("elapsedSeconds", 0)
    o:setFloat("secondsBeforeRetract", 0.01)
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    local x, y = o:getPosition()
    local state = o:getInt("state") 
    local vx, vy = 0, 0

    if state == 0 then
        local t = o:getFloat("elapsedSeconds") + elapsedSeconds
        y = easeOut(t, 800, -160, 1.5)
        o:setFloat("elapsedSeconds", t)

        if t >= 1.5 then
            o:setInt("state", 1)
        end

    elseif state == 1 then
        local secondsBeforeRetract = o:getFloat("secondsBeforeRetract") - elapsedSeconds
        o:setFloat("secondsBeforeRetract", secondsBeforeRetract)
        if secondsBeforeRetract <= 0 then
            o:setInt("state", 2)
            o:setFloat("elapsedSeconds", 0)
        end
    else
        local t = o:getFloat("elapsedSeconds") + elapsedSeconds
        y = easeIn(t, 640, 400, 1.5)
        o:setFloat("elapsedSeconds", t)
    end

    -- update position
    x = x + vx * elapsedSeconds
    y = y + vy * elapsedSeconds

    o:setPosition(x, y)
end

function onDeath(objectPointer, reason)
end