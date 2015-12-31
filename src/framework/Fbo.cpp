#include <Fbo.h>
#include <GfxSystem.h>

namespace Acidrain {

    using namespace std;

    Fbo::Fbo(int w, int h) : width(w), height(h) {
        glGenFramebuffers(1, &frameBufferId);
        glGenTextures(1, &colorBufferId);
        glGenRenderbuffers(1, &depthBufferId);

        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

        // no multisampling on the fbo
        glBindTexture(GL_TEXTURE_2D, colorBufferId);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     width, height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferId, 0);


//        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBufferId);
//        int numberOfMsaaSamples = 4;
//        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, numberOfMsaaSamples, GL_RGBA8, width, height, false);
//
//        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorBufferId, 0);

        // render buffer
        glBindRenderbuffer(GL_RENDERBUFFER, depthBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Fbo::~Fbo() {
    }

    int Fbo::getWidth() const {
        return width;
    }

    int Fbo::getHeight() const {
        return height;
    }

    GLuint Fbo::getColorBufferId() const {
        return colorBufferId;
    }

    GLuint Fbo::getDepthBufferId() const {
        return depthBufferId;
    }

    void Fbo::use() {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
        glDisable(GL_SCISSOR_TEST);
        glViewport(0, 0, width, height);
    }

    void Fbo::unuse() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GFXSYS.setViewport();
    }

    shared_ptr<Texture> Fbo::getTexture() {
        return make_shared<Texture>(colorBufferId, width, height);
    }
} // namespace Acidrain