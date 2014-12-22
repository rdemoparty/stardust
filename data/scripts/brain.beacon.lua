---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)
    o:setFloat("blastWaveDuration", 1) -- in seconds
    o:setFloat("passedTime", 0)
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    local duration = o:getFloat("blastWaveDuration")
    local t = o:getFloat("passedTime") + elapsedSeconds
    o:setFloat("passedTime", t)

    local scale = easeOut(t, 1, 1.5, duration)
    o:setScale(scale)

    local alpha = easeOut(t, 1, -1, duration)
    o:setColor(1, 1, 1, alpha)

    if t >= duration then
        o:kill(DeathReason.JobFinished)
    end
end

function onDeath(objectPointer, reason)
end

