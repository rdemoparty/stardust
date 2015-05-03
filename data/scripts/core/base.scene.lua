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

function Scene.shake(amount)
    shakeCamera(SCENE, amount)
end

function Scene.dump()
    dumpEntities(SCENE)
end
