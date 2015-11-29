#pragma once

#include <Dialog.h>

namespace Acidrain {

    using namespace std;

    class DialogRepository {
    public:
        static DialogRepository& getInstance();
        ~DialogRepository();

        int create(string message, float x, float y, Font* font, DialogOptions options);
        void update(int dialogId, float dt);
        void updateAll(float dt);
        void renderAll();
        void remove(int dialogId);

    private:
        DialogRepository();

        class impl;
        unique_ptr<impl> pimpl;
    };

} // end of namespace Acidrain
