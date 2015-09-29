---------------------------------------------------------------------
-- The scene abstraction
---------------------------------------------------------------------

Scene = {}
function Scene.create(type, parent)
    local entityPointer
    if not parent then
        entityPointer = createEntity(type, NULL_POINTER)
    else
        entityPointer = createEntity(type, parent:getPointer())
    end

    return Entity.from(entityPointer)
end

function Scene.confineToPlayingArea(entity)
    return confineToPlayingArea(entity:getPointer())
end

function Scene.dump()
    dumpEntities()
end
