---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------

SQRT2 = math.sqrt(2)

function onSpawn(objectPointer)
    local o = Entity.from(objectPointer)
    o:setFloat("mass", 10)

    o:setFloat("thrusterForce", 30000)

    o:setFloat("vx", 0)
    o:setFloat("vy", 0)

    o:setFloat("ax", 0)
    o:setFloat("ay", 0)

    o:setFloat("friction", 0.95)

    o:setFloat("maxSpeed", 600)
end

function calculateForces(thrusterForce)
    local forceX = 0
    local forceY = 0

    if leftPressed() then
    	forceX = -thrusterForce
    elseif rightPressed() then
    	forceX = thrusterForce
    end

    if upPressed() then
    	forceY = -thrusterForce
    elseif downPressed() then
    	forceY = thrusterForce
    end

    return forceX, forceY
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    if fireJustPressed() then
    	o:fire(true)
    elseif fireJustReleased() then
    	o:fire(false)
    end

    -- calculate all forces acting upon the ship
    local thrusterForce = o:getFloat("thrusterForce")
    local forceX, forceY = calculateForces(thrusterForce)

    -- figure out acceleration: a = F/m
    local mass = o:getFloat("mass")
    local accelX = forceX / mass
    local accelY = forceY / mass

    -- update velocity
    local vx = o:getFloat("vx")
    local vy = o:getFloat("vy")

    vx = vx + accelX * elapsedSeconds
    vy = vy + accelY * elapsedSeconds

    -- cap velocity to a max value
    local currentSpeed = math.sqrt(vx*vx + vy*vy)
    local maxSpeed = o:getFloat("maxSpeed")
    if currentSpeed > maxSpeed then
        local ratio = maxSpeed / currentSpeed
        vx = vx * ratio
        vy = vy * ratio
    end

    -- apply friction
    local friction = o:getFloat("friction")
    vx = vx * friction
    vy = vy * friction

    -- update position
    local x, y = o:getPosition()

    x = x + vx * elapsedSeconds
    y = y + vy * elapsedSeconds

    -- persist position and velocity
    o:setPosition(x, y)

    -- keep ship on screen and reset velocity if it hit a boundary
    if Scene.confineToPlayingArea(o) then
        -- vx = 0
        -- vy = 0
    end

    o:setFloat("vx", vx)
    o:setFloat("vy", vy)
end


function onDeath(objectPointer, reason)
end

