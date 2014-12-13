---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    if o:isAnimationFinished() then
        o:kill(DeathReason.JobFinished)
    end
end

function onDeath(objectPointer, reason)
    local killedEntity = Entity.from(objectPointer)

    local explosionLevel = killedEntity:getInt("explosion.level")
    if explosionLevel < 1 then
        for i = 0, 4, 1 do
            local xpl = Scene.create("enemy.death.explosion")
            local x = 30 * math.sin((i / 2.0) * math.pi)
            local y = 30 * math.cos((i / 2.0) * math.pi)
            local xx, yy = killedEntity:getPosition()
            xpl:setPosition(x + xx, y + yy)
            xpl:setInt("explosion.level", explosionLevel + 1)
            Scene.add(xpl)
        end
    end

end

