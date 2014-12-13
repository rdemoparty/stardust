function onUpdate (entity, elapsedSeconds)
  fireWeapons(entity, true);

  speed = getFloat(entity, "speed")
  -- first time speed is not set so we're forcefully setting it 
  setFloat(entity, "speed", 200)

  phase = getFloat(entity, "phase") + elapsedSeconds
  setFloat(entity, "phase", phase)

  rotation = getRotation(entity)
  rotation = rotation + 5.0 * elapsedSeconds
  setRotation(entity, rotation)

  setColor(entity, 1, 1, 1, 0.3 * math.sin(phase*2) + 0.6)

  setScale(entity, 0.6 * math.sin(phase*5) + 1)

  x, y = getPosition(entity)
  setPosition(entity, 300 * math.sin(phase) + 300, y + speed * elapsedSeconds)
end
