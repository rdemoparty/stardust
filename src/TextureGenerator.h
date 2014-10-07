#pragma once
#include <Texture.h>
#include <memory>

namespace Acidrain {

class TextureGenerator {
public:
    TextureGenerator(int width, int height);
    virtual ~TextureGenerator();

    TextureGenerator& envMap(unsigned char layer, unsigned char size);
    TextureGenerator& lens(unsigned char layer, unsigned char size);

    TextureGenerator& roll(unsigned char layer, unsigned char x, unsigned char y);
    // TextureGenerator& polarGrid(unsigned char layer);

    unsigned char* get(unsigned char layer);
    std::shared_ptr<Texture> getTexture(unsigned char layer);

    int width;
    int height;

private:
    static const int NUM_LAYERS = 4;
    unsigned char* layers[NUM_LAYERS];
};

} // namespace Acidrain
