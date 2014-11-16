#include <Collisions.h>
#include <GfxSystem.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Acidrain {

    // Circle

    Circle::Circle(float r, vec2 c)
            : radius(r), center(c), worldRadius(r), worldCenter(c) {
    }

    void Circle::transform(mat4 parentTransform) {
        vec4 transformedCenter = parentTransform * vec4(center.x, center.y, 0, 1);
        worldCenter = vec2(transformedCenter.x, transformedCenter.y);
        worldRadius = radius * parentTransform[2][2]; // we assume scale is uniform and we just take one component
    }

    void Circle::draw() {
        GFXSYS.drawCircle(worldCenter, worldRadius, glm::vec4(1));
    }

    // Collision hull

    void CollisionHull::add(Circle circle) {
        circles.push_back(circle);

        float currentCircleFarthestPoint = length(circle.center) + circle.radius;
        if (currentCircleFarthestPoint > overallRadius)
            overallRadius = currentCircleFarthestPoint;
    }

    void CollisionHull::transform(mat4 parentTransform) {
        for (auto &circle : circles)
            circle.transform(parentTransform);
    }

    void CollisionHull::draw() {
        for (auto& circle : circles)
            circle.draw();
    }
}