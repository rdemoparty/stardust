---------------------------------------------------------------------
-- The game session abstraction
---------------------------------------------------------------------

Session = {}
function Session:setFloat(name, value)
    setSessionFloat(GAME_SESSION, name, value)
end

function Session:getFloat(name)
    return getSessionFloat(GAME_SESSION, name)
end

function Session:setInt(name, value)
    setSessionInt(GAME_SESSION, name, value)
end

function Session:getInt(name)
    return getSessionInt(GAME_SESSION, name)
end
