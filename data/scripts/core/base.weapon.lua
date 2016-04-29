---------------------------------------------------------------------
-- A simple weapon abstraction used by simple game objects
---------------------------------------------------------------------

Weapon = {} -- the table representing the class, which will double as the metatable for the instances
Weapon.__index = Weapon -- failed table lookups on the instances should fallback to the class table, to get methods

function Weapon.new(shotsPerSecond, soundWhenFired)
    local self = setmetatable({}, Weapon)
    self.shotsPerSecond = shotsPerSecond
    self.soundWhenFired = soundWhenFired
    self.emitters = {}
    return self
end

function Weapon:addEmitter(bulletType, x, y)
    table.insert(self.emitters, {
        bullet = bulletType,
        x = x,
        y = y
    })
end

function Weapon:printEmitters()
    for k,v in pairs(self.emitters) do
        print ("Bullet of type" .. v.bullet .. " at " .. v.x .. "," .. v.y)
    end
end
