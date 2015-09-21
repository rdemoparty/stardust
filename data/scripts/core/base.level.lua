---------------------------------------------------------------------
-- The level abstraction
---------------------------------------------------------------------

Level = {}

function Level.finish()
    endLevel(SCENE)
end

function Level.pause()
    pauseLevel(SCENE)
end

function Level.resume()
    resumeLevel(SCENE)
end
