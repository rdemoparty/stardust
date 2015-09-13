include("scripts/core/base.scene.lua")
include("scripts/core/base.entity.lua")
include("scripts/core/base.math.lua")
include("scripts/core/base.session.lua")
include("scripts/core/base.camera.lua")

-- default brain implementations. feel free to override
function onSpawn(objectPointer)
end

function onUpdate(objectPointer, elapsedSeconds)
end

function onDamage(objectPointer, amount, inflicterPointer)
end

function onDeath(objectPointer, reason)
end

