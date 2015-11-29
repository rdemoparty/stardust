#include <DialogRepository.h>
#include <map>

namespace Acidrain {

    class DialogRepository::impl {
    public:

        int create(string message, float x, float y, Font* font, DialogOptions options);
        void update(int dialogId, float dt);
        void renderAll();
        void updateAll(float dt);
        void remove(int dialogId);

        int dialogId = 1;
        map<int, Dialog*> dialogs;
    };

    DialogRepository& DialogRepository::getInstance() {
        static DialogRepository instance;
        return instance;
    }

    DialogRepository::DialogRepository()
            : pimpl(new DialogRepository::impl()) {
    }

    DialogRepository::~DialogRepository() {
        // killall dialogs
    }

    int DialogRepository::create(string message, float x, float y, Font* font, DialogOptions options) {
        return pimpl->create(message, x, y, font, options);
    }

    void DialogRepository::update(int dialogId, float dt) {
        pimpl->update(dialogId, dt);
    }

    void DialogRepository::remove(int dialogId) {
        pimpl->remove(dialogId);
    }

    void DialogRepository::renderAll() {
        pimpl->renderAll();
    }

    void DialogRepository::updateAll(float dt) {
        pimpl->updateAll(dt);
    }


    // ---------------------------------------------------------------------------

    int DialogRepository::impl::create(string message, float x, float y, Font* font, DialogOptions options) {
        dialogs[dialogId] = new Dialog(message, x, y, font, options);
        int result = dialogId++;
        return result;
    }

    void DialogRepository::impl::update(int dialogId, float dt) {
        if (dialogs.lower_bound(dialogId) != dialogs.end()) {
            dialogs[dialogId]->update(dt);
        }
    }

    void DialogRepository::impl::remove(int dialogId) {
        auto it = dialogs.find(dialogId);
        if (it != dialogs.end()) {
            delete it->second;
            dialogs.erase(it);
        }
    }

    void DialogRepository::impl::renderAll() {
        for (auto& kv : dialogs) {
            kv.second->render();
        }
    }

    void DialogRepository::impl::updateAll(float dt) {
        for (auto& kv : dialogs) {
            kv.second->update(dt);
        }
    }


} // end of namespace Acidrain