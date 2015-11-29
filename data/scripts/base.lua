include("scripts/core/base.scene.lua")
include("scripts/core/base.level.lua")
include("scripts/core/base.entity.lua")
include("scripts/core/base.math.lua")
include("scripts/core/base.session.lua")
include("scripts/core/base.camera.lua")
include("scripts/core/base.dialog.lua")

-- default brain implementations. feel free to override
function onSpawn(objectPointer)
end

function onUpdate(objectPointer, elapsedSeconds)
end

function onDamage(objectPointer, amount, inflicterPointer)
    inflictDamage(objectPointer, amount, inflicterPointer)
end

function onDeath(objectPointer, reason)
end

function inflictDamage(objectPointer, amount, inflicterPointer)
    local self = Entity.from(objectPointer)

    local life = self:getLife() - amount
    if life < 0 then
        life = 0
    end
    self:setLife(life)

    if life <= 0 then
        self:kill(DeathReason.Killed)
    end
end

