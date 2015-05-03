-- cubic easing
function easeInCubic(time, startValue, changeInValue, duration)
    t = time / duration
    return changeInValue * t * t * t + startValue
end

function easeOutCubic(time, startValue, changeInValue, duration)
    t = time / duration - 1
    return changeInValue * (t*t*t + 1) + startValue
end

-- quintic easing
function easeInQuintic(time, startValue, changeInValue, duration)
    t = time / duration
    return changeInValue * t * t * t * t * t + startValue 
end

function easeOutQuintic(time, startValue, changeInValue, duration)
    t = time / duration - 1
    return changeInValue * (t*t*t*t*t + 1) + startValue 
end
