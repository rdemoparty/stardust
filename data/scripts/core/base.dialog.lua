---------------------------------------------------------------------
-- Entrypoint for dialog handling related functions
---------------------------------------------------------------------

Dialog = {}
function Dialog.create(parameters)
    return createDialog(parameters)
end

function Dialog.update(dialogId, elapsedSeconds)
    updateDialog(dialogId, elapsedSeconds)
end

function Dialog.remove(dialogId)
    removeDialog(dialogId)
end
