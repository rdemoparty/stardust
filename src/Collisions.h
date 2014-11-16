#pragma once

#define GLM_FORCE_RADIANS
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

namespace Acidrain {

    using namespace glm;
    using namespace std;

    class Circle {
    public:
        Circle(float radius, vec2 center=vec2(0));

        void transform(mat4 parentTransform);

        void draw();

    public:

        float radius;
        vec2 center;

        float worldRadius;
        vec2 worldCenter;
    };

    class CollisionHull {
    public:
        void add(Circle circle);

        void transform(mat4 parentTransform);

        void draw();

    private:
        float overallRadius;
        vector<Circle> circles;
    };

} // namespace Acidrain