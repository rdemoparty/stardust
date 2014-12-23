---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    -- if o:isAnimationFinished() then
    --     o:kill(DeathReason.JobFinished)
    -- end
end

function onDeath(objectPointer, reason)
    local killedEntity = Entity.from(objectPointer)

    local beacon = Scene.create("beacon")
    beacon:setPosition(killedEntity:getPosition())
    Scene.add(beacon)
    

    -- local bonus = Scene.create("missile")
    -- bonus:setPosition(killedEntity:getPosition())
    -- Scene.add(bonus)
end

