---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)
    playSound("sounds/rain/enemy_ships_detected_ahead.ogg", "PLAYER")
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    -- update position
    local x, y = o:getPosition()
    local t = o:getFloat("elapsedSeconds") + elapsedSeconds
    o:setFloat("elapsedSeconds", t)

    y = easeInQuintic(t, 800, -260, 2)

    o:setPosition(x, y)

    -- persist position and velocity
    if t >= 2 then
        o:setBrain("scripts/brain.player.lua")
    end
end

function onDeath(objectPointer, reason)
end