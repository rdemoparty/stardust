---------------------------------------------------------------------
-- The game session abstraction
---------------------------------------------------------------------

Session = {}
function Session.setFloat(name, value)
    setSessionFloat(name, value)
end

function Session.getFloat(name)
    return getSessionFloat(name)
end

function Session.setInt(name, value)
    setSessionInt(name, value)
end

function Session.getInt(name)
    return getSessionInt(name)
end
