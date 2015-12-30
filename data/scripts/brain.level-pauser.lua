---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------
function onSpawn(objectPointer)
    Level.pause()

    local o = Entity.from(objectPointer)
    local dialogId = Dialog.create {
        message = "Kill the flashing explosion \nin order to resume the level ",
        x = 500,
        y = 150,
        font = {
            face = "fonts/alien.ttf",
            size = 24
        },
        options = {
            scrollOldLinesUp = false,
            charColor = {
                r = 0.45,
                b = 0.0,
                g = 0.9,
                a = 1
            },
            activeCharColor = {
                r = 0.75,
                b = 0.5,
                g = 1,
                a = 1
            }
        }
    }
    o:setInt("dialogId", dialogId)
end

function onUpdate(objectPointer, elapsedSeconds)
    local o = Entity.from(objectPointer)

    local x, y = o:getPosition()
    y = y + 100 * elapsedSeconds
    o:setPosition(x, y)
end


function onDeath(objectPointer, reason)
    local o = Entity.from(objectPointer)
    Dialog.remove(o:getInt("dialogId"))
    Level.resume()
end

