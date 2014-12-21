---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)

    ------ memleak settings    
    o:setFloat("mass", 6)
 
    o:setFloat("thrusterForce", 40000)
 
    o:setFloat("vx", 0)
    o:setFloat("vy", 0)
 
    o:setFloat("ax", 0)
    o:setFloat("ay", 0)
 
    o:setFloat("friction", 0.80)
 
    o:setFloat("maxSpeed", 800)    
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)


    -- update velocity
    local vx = 0
    local vy = -o:getFloat("maxSpeed") / 4

    -- update position
    local x, y = o:getPosition()

    x = x + vx * elapsedSeconds
    y = y + vy * elapsedSeconds

    -- persist position and velocity
    if y < 600 then
        o:setPosition(x, y)

        o:setFloat("vx", 0)
        o:setFloat("vy", 0)
        o:setBrain("scripts/brain.player.lua")
    else
        o:setPosition(x, y)

        o:setFloat("vx", vx)
        o:setFloat("vy", vy)
    end

end

function onDeath(objectPointer, reason)
end