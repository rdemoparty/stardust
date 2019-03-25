#pragma once

#include <GameState.h>
#include <memory>

namespace Acidrain {

    class Stardust;
    class Fbo;
    class GpuProgram;
    class GpuProgramConstantBundle;
    class Texture;

    class GameStatePlayLevel : public GameState<Stardust> {
    public:
        static GameStatePlayLevel &instance();

        const char* name() const override { return "Play Level"; }
        void onEnter(Stardust *game) override;
        void onExit(Stardust *game) override;
        void update(Stardust *game, float elapsedSeconds) override;
        void render(Stardust *game, float alpha) override;

    private:
        GameStatePlayLevel();

        void handleGameEvents(const Stardust *game) const;
        float totalElapsedTime = 0;
        int renderMode = 0;
        std::shared_ptr<Fbo> fbo;
        std::shared_ptr<Texture> fboTexture;
        std::shared_ptr<GpuProgram> gpuProgram;
        std::shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;
    };

} // namespace Acidrain