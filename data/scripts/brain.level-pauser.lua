---------------------------------------------------------------------
-- The brain functions
---------------------------------------------------------------------
function onSpawn(objectPointer)
    Level.pause()

    local o = Entity.from(objectPointer)
    local dialogId = Dialog.create {
        message = "Kill the flashing explosion \nin order to resume the level ",
        x = 500,
        y = 100,
        font = {
            face = "fonts/Neo Sans Pro Medium.ttf",
            size = 30
        },
        options = {
            scrollOldLinesUp = false,
            charColor = {
                r = 0.6,
                g = 0.2,
                b = 0.1,
                a = 1
            },
            activeCharColor = {
                r = 0.8,
                g = 0.8,
                b = 0.8,
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

