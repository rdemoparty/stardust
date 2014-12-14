DeathReason = {
    OutOfVisibleArea = 1,
    Killed = 2,
    JobFinished = 3
}

Entity = {} -- the table representing the class, which will double as the metatable for the instances
Entity.__index = Entity -- failed table lookups on the instances should fallback to the class table, to get methods

function Entity.from(pointer)
    local self = setmetatable({}, Entity)
    self.pointer = pointer
    return self
end

function Entity:fire(shouldFire)
    fireWeapons(self.pointer, shouldFire)
end

function Entity:kill(reason)
    kill(self.pointer, reason)
end

function Entity:isAnimationFinished()
    return isAnimationFinished(self.pointer)
end

function Entity:getPosition()
    return getPosition(self.pointer)
end

function Entity:setPosition(x, y)
    setPosition(self.pointer, x, y)
end

function Entity:setRotation(amount)
    setRotation(self.pointer, amount)
end

function Entity:getRotation()
    return getRotation(self.pointer)
end

function Entity:setColor(r, g, b, a)
    setColor(self.pointer, r, g, b, a)
end

function Entity:setScale(amount)
    setScale(self.pointer, amount)
end

function Entity:getScale()
    return getScale(self.pointer)
end

function Entity:getPointer()
    return self.pointer
end

function Entity:setFloat(name, value)
    setFloat(self.pointer, name, value)
end

function Entity:getFloat(name)
    return getFloat(self.pointer, name)
end

function Entity:setInt(name, value)
    setInt(self.pointer, name, value)
end

function Entity:getInt(name)
    return getInt(self.pointer, name)
end

---------------------------------------------------------------------
-- The scene abstraction
---------------------------------------------------------------------

Scene = {}
function Scene.create(type)
    local entityPointer = createEntity(SCENE, type);
    return Entity.from(entityPointer)
end

function Scene.add(entity)
    addEntity(SCENE, entity:getPointer())
end

function Scene.confineToPlayingArea(entity)
    return confineToPlayingArea(SCENE, entity:getPointer())
end

function Scene.dump()
    dumpEntities(SCENE)
end

