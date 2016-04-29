---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)

    x, y = o:getPosition()

    o:setFloat("center", x)
    o:setFloat("speed", 200)
--    o:setFloat("aperture", math.random(100, 400))
--    o:setFloat("phase", math.random() * math.pi * 2.0)
    o:setFloat("aperture", 200)
    o:setFloat("phase", 0.0)
    o:setFloat("hitTimer", 0)
    o:setFloat("hitDuration", 0.05)

    w = Weapon.new(2, "")
    w:addEmitter("enemy.bullet", 0, 32)
    o:addWeapon(w)

    o:fire(true)
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    local phase = o:getFloat("phase") + elapsedSeconds
    o:setFloat("phase", phase)

    -- make it colored when taking damage for more visual feedback
    local hitTimer = o:getFloat("hitTimer")
    if hitTimer > 0 then
        hitTimer = hitTimer - elapsedSeconds
        if hitTimer < 0 then
            hitTimer = 0
        end
        o:setFloat("hitTimer", hitTimer)

        local duration = o:getFloat("hitDuration")
        local rValue = 1
        local gValue = easeInQuintic(hitTimer, 0, 1, duration)
        local bValue = easeInQuintic(hitTimer, 0, 1, duration)
        -- gValue, bValue = 0, 0
        o:setColor(rValue, gValue, bValue, 1)
    else
        o:setColor(1, 1, 1, 1)
    end

    -- o:setRotation(o:getRotation() + 5.0 * elapsedSeconds)
    -- o:setColor(1, 1, 1, 0.3 * math.sin(phase*2) + 0.6)
    -- o:setScale(0.6 * math.sin(phase*5) + 1)

    local speed = o:getFloat("speed")
    local x, y = o:getPosition()
    x = o:getFloat("aperture") * math.sin(phase) + o:getFloat("center")
    y = y + speed * elapsedSeconds
    o:setPosition(x, y)
end

function onDamage(objectPointer, amount, inflicterPointer)
    inflictDamage(objectPointer, amount, inflicterPointer)

    local hurtEntity = Entity.from(objectPointer)
    local inflictedEntity = Entity.from(inflicterPointer)

    hurtEntity:setFloat("hitTimer", hurtEntity:getFloat("hitDuration"))
end

function onDeath(objectPointer, reason)
    local killedEntity = Entity.from(objectPointer)
    if reason == DeathReason.Killed then

        playSound("sounds/fire_explosion.ogg", "EXPLOSIONS");

        local explosion = Scene.create("enemy.death.explosion")
        explosion:setPosition(killedEntity:getPosition())

        local beacon = Scene.create("beacon")
        beacon:setPosition(killedEntity:getPosition())

        if math.random(10) > 7 then
            local bonusType = 1
            local bonus = Scene.create("powerup" .. bonusType)
            bonus:setInt("bonusType", bonusType)
            bonus:setPosition(killedEntity:getPosition())
        end
    end
end

