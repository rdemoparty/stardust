include("scripts/core/base.scene.lua")
include("scripts/core/base.entity.lua")
include("scripts/core/base.math.lua")

-- default brain implementations. feel free to override
function onSpawn(objectPointer)
end

function onUpdate(objectPointer, elapsedSeconds)
end

function onDamage(objectPointer, amount, inflicterPointer)
end

function onDeath(objectPointer, reason)
end

