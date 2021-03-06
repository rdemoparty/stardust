#include <TextureGenerator.h>
#include <cmath>
#include <cstring>
#include <glm/vec4.hpp>

namespace Acidrain {

    using namespace std;
    using namespace glm;


    typedef glm::vec4 Colour;

    inline uint32_t asInt(const glm::vec4& col) {
        unsigned char rr = static_cast<unsigned char>(col.r * 255);
        unsigned char gg = static_cast<unsigned char>(col.g * 255);
        unsigned char bb = static_cast<unsigned char>(col.b * 255);
        unsigned char aa = static_cast<unsigned char>(col.a * 255);
        return rr + (gg << 8) + (bb << 16) + (aa << 24);
    }


    TextureGenerator::TextureGenerator(int w, int h)
            : width(w), height(h) {

        for (int i = 0; i < NUM_LAYERS; i++)
            layers[i] = new unsigned char[width * height * 4];
    }

    TextureGenerator::~TextureGenerator() {
        for (int i = 0; i < NUM_LAYERS; i++)
            if (layers[i] != nullptr)
                delete[] layers[i];
    }

    TextureGenerator& TextureGenerator::envMap(unsigned char layer, unsigned char size) {
        uint32_t* input = reinterpret_cast<uint32_t*>(layers[layer]);

        Colour col = {1, 1, 1, 1};

        int sizeSquared = size * size;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int rx = x - width / 2;
                int ry = y - height / 2;
                int rsquare = rx * rx + ry * ry;

                if (rsquare <= sizeSquared) {
                    double a = 1.0 - (rsquare / static_cast<double>(sizeSquared));
                    col.r = col.g = col.b = a;
                    // col.a = 1;
                } else {
                    col.r = col.g = col.b = 0;
                    // col.a = 1;
                }

                *input++ = asInt(col);
            }
        }

        return *this;
    }


    TextureGenerator& TextureGenerator::lens(unsigned char layer, unsigned char size) {
        uint32_t* input = reinterpret_cast<uint32_t*>(layers[layer]);

        Colour col = vec4(0);

        int sizeSquared = size * size;
        double c;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int rx = x - width / 2;
                int ry = y - height / 2;
                int rsquare = rx * rx + ry * ry;

                if (rsquare <= sizeSquared) {
                    double r = sqrt(static_cast<double>(rsquare)) / static_cast<double>(size);
                    // double r  = 0;
                    c = 1 - r;
                    c = c * c;
                    if (r > 1)
                        c = 0;
                    col = vec4(c);
                } else {
                    col = vec4(0);
                }

                *input++ = asInt(col);
            }
        }

        return *this;
    }


    TextureGenerator& TextureGenerator::roll(unsigned char layer, unsigned char x, unsigned char y) {
        unsigned char* input = layers[layer];
        unsigned char* output = new unsigned char[width * height * 4];
        unsigned char* outptr = output;

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int index = ((j + y) % height) * width + ((i + x) % width);
                *outptr++ = input[index * 4 + 0];
                *outptr++ = input[index * 4 + 1];
                *outptr++ = input[index * 4 + 2];
                *outptr++ = input[index * 4 + 3];
            }
        }

        memcpy(input, output, width * height * 4);
        delete[] output;

        return *this;
    }


    unsigned char* TextureGenerator::get(unsigned char layer) {
        return layers[layer];
    }

    shared_ptr<Texture> TextureGenerator::getTexture(unsigned char layer) {
        return shared_ptr<Texture>(new Texture(width, height, get(layer)));
    }

// TextureGenerator& TextureGenerator::polarGrid(unsigned char layer) {
//     unsigned char* input = layers[layer];

//     unsigned char* output = new unsigned char[width * height * 4];
//     unsigned char* outptr = output;

//     for (int j = 0; j < height; j++) {
//         double theta = M_PI * (j - (height - 1) / 2.0) / static_cast<double>(height - 1);
//         for (int i = 0; i < width; i++) {
//             double phi  = 2 * M_PI * (i - width / 2.0) / static_cast<double>(width);
//             double phi2 = phi * cos(theta);
//             int i2  = phi2 * width / (2 * M_PI) + width / 2;
//             if (i2 < 0 || i2 > width - 1) {
//                 *outptr++ = 255;
//                 *outptr++ = 0;
//                 *outptr++ = 0;
//                 *outptr++ = 1;

//             } else {
//                 *outptr++ = input[(j * width + i2) * 4 + 0];
//                 *outptr++ = input[(j * width + i2) * 4 + 1];
//                 *outptr++ = input[(j * width + i2) * 4 + 2];
//                 *outptr++ = input[(j * width + i2) * 4 + 3];
//             }
//         }
//     }

//     memcpy(input, output, width * height * 4);
//     delete [] output;

//     return *this;
// }


} // namespace Acidrain 

