#include <Stardust.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>

namespace Acidrain {

    struct Rectangle {
        glm::vec2 tl = glm::vec2(-0.5, 0.5);
        glm::vec2 br = glm::vec2(0.5, -0.5);

        Rectangle(int width, int height) {
            tl *= width;
            br *= height;
        }
    };

    Stardust::Stardust() {
        shader = std::shared_ptr<Shader>(new Shader(
                FileSystem::getFileContent("data/shaders/normal.vs").c_str(),
                FileSystem::getFileContent("data/shaders/normal.ps").c_str()
        ));

        texture = GFXSYS.loadTexture("/home/benny/archives/work/stardust/data/menu/new_game_selected.png");
    }

    Stardust::~Stardust() {
    }

    void Stardust::process(double elapsedSeconds) {
        glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(1.0f, 1.0f, 1.0f);

        glEnable(GL_TEXTURE_2D);
        texture->use();

        const int WIDTH = 250;
        const int HEIGHT = 50;

        const int X_POS = 0;
        const int Y_POS = 0;

        glEnable(GL_BLEND);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f(X_POS, Y_POS, 0);

        glTexCoord2f(0, 0);
        glVertex3f(0, HEIGHT, 0);

        glTexCoord2f(1, 0);
        glVertex3f(WIDTH, HEIGHT, 0);

        glTexCoord2f(1, 1);
        glVertex3f(WIDTH, 0, 0);

        glEnd();
    }

    bool Stardust::shouldQuit() {
        return false;
    }
} // namespace Acidrain