---------------------------------------------------------------------
-- The scene abstraction
---------------------------------------------------------------------

Scene = {}
function Scene.create(type)
    local entityPointer = createEntity(type);
    return Entity.from(entityPointer)
end

function Scene.confineToPlayingArea(entity)
    return confineToPlayingArea(SCENE, entity:getPointer())
end

function Scene.dump()
    dumpEntities(SCENE)
end

function Scene.endLevel()
    endLevel(SCENE)
end

function Scene.pauseLevel()
    pauseLevel(SCENE)
end

function Scene.resumeLevel()
    resumeLevel(SCENE)
end
