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
        Circle(float radius, vec2 center = vec2(0));

        void transform(mat4 parentTransform);

        void draw();

        bool collidesWith(Circle& other);

    public:

        float radius;
        vec2 center;

        float worldRadius;
        vec2 worldCenter;

        float previousWorldRadius;
        vec2 previousWorldCenter;
    };

    class CollisionHull {
    public:
        void add(Circle circle);

        void transform(mat4 parentTransform);

        void draw();

        bool collidesWith(CollisionHull& other);

    private:
        // The root circle that encompasses all other collision circles.
        // This one is used for early out collision detections.
        Circle boundingCircle = {0, vec2(0, 0)};
        vector<Circle> circles;
    };

} // namespace Acidrain