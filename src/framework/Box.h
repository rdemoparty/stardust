#pragma once

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace Acidrain {

    using namespace glm;
    using namespace std;

    struct Box {
        vec2 tl = vec2(-0.5, 0.5);
        vec2 br = vec2(0.5, -0.5);

        Box() {
        }

        Box(float x, float y, float width, float height) {
            tl = vec2(x, y);
            br = tl + vec2(width, height);
        }

        Box(const vec2& size) {
            tl = vec2(0, 0);
            br = vec2(size.x, size.y);
            centerAround(vec2(0, 0));
        }

        Box(float width, float height) {
            tl = vec2(0, 0);
            br = vec2(width, height);
            centerAround(vec2(0, 0));
        }

        Box(const Box& other) {
            tl = other.tl;
            br = other.br;
        }

        Box& operator=(const Box& other) {
            tl = other.tl;
            br = other.br;
            return *this;
        }

        float width() const {
            return br.x - tl.x;
        }

        float height() const {
            return br.y - tl.y;
        }

        float top() const {
            return tl.y;
        }

        float right() const {
            return br.x;
        }

        float bottom() const {
            return br.y;
        }

        float left() const {
            return tl.x;
        }

        vec2 getCenter() const {
            return (tl + br) / 2.0f;
        }

        void translate(const vec2& amount) {
            tl += amount;
            br += amount;
        }

        void centerAround(const vec2& position) {
            translate(position - getCenter());
        }

        void normalizeForSize(const int width, const int height) {
            tl.x /= static_cast<float>(width);
            tl.y /= static_cast<float>(height);

            br.x /= static_cast<float>(width);
            br.y /= static_cast<float>(height);
        }

        vector<vec2> computeVertices() {
            return vector<vec2> {
                    tl,
                    vec2(br.x, tl.y),
                    br,
                    vec2(tl.x, br.y)
            };
        }
    };

} // namespace Acidrain