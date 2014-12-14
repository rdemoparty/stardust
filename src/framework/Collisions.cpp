#include <Collisions.h>
#include <GfxSystem.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Acidrain {

    // Circle

    Circle::Circle(float r, vec2 c)
            : radius(r), center(c), worldRadius(r), worldCenter(c) {
    }

    void Circle::transform(mat4 parentTransform) {
        previousWorldCenter = worldCenter;
        previousWorldRadius = worldRadius;

        vec4 transformedCenter = parentTransform * vec4(center.x, center.y, 0, 1);
        worldCenter = vec2(transformedCenter.x, transformedCenter.y);
        worldRadius = radius * parentTransform[2][2]; // we assume scale is uniform and we just take one component

        // The code below fixes a subtle bug:
        // - if the entity is created at large x, y, previous coordinates are 0
        //   and collision detection code will take the huge velocities into account.
        //   This way we can get false detections between entities that are nowhere near,
        //   but the collision detection code assumes they collided somewhere between last
        //   and current frame.
        //
        //   We fix this by setting previous state equal to the current state for first frame.
        if (firstTransform) {
            previousWorldCenter = worldCenter;
            previousWorldRadius = worldRadius;
        }

        firstTransform = false;
    }

    void Circle::draw() {
        GFXSYS.drawLine(previousWorldCenter, worldCenter, glm::vec4(1, 1, 1, 0.3));
        GFXSYS.drawCircle(worldCenter, worldRadius, glm::vec4(1, 1, 1, 0.3));
    }

    bool Circle::collidesWith(Circle& other) {
        vec2 diff = other.worldCenter - this->worldCenter;

        // currently colliding ?
        double c = dot(diff, diff) - (this->radius + other.radius) * (this->radius + other.radius);
        if (c < 0)
            return true;

        vec2 velocityA = this->worldCenter - this->previousWorldCenter;
        vec2 velocityB = other.worldCenter - other.previousWorldCenter;

        // test if circles not moving relative each other
        vec2 velocityDifference = velocityB - velocityA;
        double aa = dot(velocityDifference, velocityDifference);
        if (aa < 0.0)
            return false;

        // circles moving apart ?
        double bb = dot(velocityDifference, diff);
        if (bb >= 0)
            return false;

        // check if we have solution to the quadratic equation
        double d = bb * bb - aa * c;
        if (d < 0)
            return false;

        // evaluate the time of collision
        double t = (-bb - ::sqrt(d)) / aa;
        return t <= 1;
    }

    // Collision hull

    void CollisionHull::add(Circle circle) {
        circles.push_back(circle);

        float currentCircleFarthestPoint = length(circle.center) + circle.radius;
        if (currentCircleFarthestPoint > boundingCircle.radius)
            boundingCircle.radius = currentCircleFarthestPoint;
    }

    void CollisionHull::transform(mat4 parentTransform) {
        boundingCircle.transform(parentTransform);
        for (auto& circle : circles)
            circle.transform(parentTransform);
    }

    void CollisionHull::draw() {
        boundingCircle.draw();
        for (auto& circle : circles)
            circle.draw();
    }

    bool CollisionHull::collidesWith(CollisionHull& other) {
        if (boundingCircle.collidesWith(other.boundingCircle)) {
            for (auto& circle : circles) {
                for (auto& otherCircle : other.circles) {
                    if (circle.collidesWith(otherCircle)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
