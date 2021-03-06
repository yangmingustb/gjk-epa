#include <iostream>

#include <collision/GJKCollisionDetector.h>
#include <geometry/Circle.h>
#include <geometry/Polygon.h>
#include <math/Vector2Util.h>

using std::cout;
using std::endl;

int main()
{
    unique_ptr<Polygon> triangle = Polygon::createTriangle(
            fvec2(4.0f, 11.0f),
            fvec2(4.0f, 5.0f),
            fvec2(9.0f, 9.0f));
    Transform2 triangleTransform;

    unique_ptr<Polygon> polygon = Polygon::createPolygon({
            fvec2(5.0f, 7.0f),
            fvec2(7.0f, 3.0f),
            fvec2(10.0f, 2.0f),
            fvec2(12.0f, 7.0f)});
    Transform2 polygonTransform;

    unique_ptr<Polygon> rectangle = Polygon::createRectangle(10.0, 12.0);
    Transform2 rectangleTransform;

    unique_ptr<Circle> circle = Circle::create(2.0);
    Transform2 circleTransform;
    Transform2 circleTransform2;
    circleTransform2.translate(1.0, 1.2);


    GJKCollisionDetector collisionDetector;

    // Detect collision between triangle and polygon, should be a collision
    Penetration penetration;
    bool collisionTP = collisionDetector.detect(*triangle, triangleTransform, *polygon, polygonTransform, penetration);
    cout << "Do we have a collision between triangle and polygon: " << collisionTP;
    cout << ", penetration normal: (" << penetration.normal.x << "," << penetration.normal.y << ")";
    cout << ", penetration depth: " << penetration.depth << endl;

    // Detect collision between polygon and rectangle, should *not* be a collision
    bool collisionPR = collisionDetector.detect(*polygon, polygonTransform, *rectangle, rectangleTransform);
    cout << "Do we have a collision between polygon and rectangle: " << collisionPR << endl;

    // Detect collision between triangle and rectangle, should be a collision
    Penetration penetrationTR;
    bool collisionTR = collisionDetector.detect(*triangle, triangleTransform, *rectangle, rectangleTransform, penetrationTR);
    cout << "Do we have a collision between triangle and rectangle: " << collisionTR;
    cout << ", penetration normal: (" << penetrationTR.normal.x << "," << penetrationTR.normal.y << ")";
    cout << ", penetration depth: " << penetrationTR.depth << endl;

    // Detect collision between rectangle and circle, should be a collision
    Penetration penetrationRC;
    bool collisionRC = collisionDetector.detect(*rectangle, rectangleTransform, *circle, circleTransform, penetrationRC);
    cout << "Do we have a collision between rectangle and circle: " << collisionRC;
    cout << ", penetration normal: (" << penetrationRC.normal.x << "," << penetrationRC.normal.y << ")";
    cout << ", penetration depth: " << penetrationRC.depth << endl;

    // Detect collision between triangle and circle, should *not* be a collision
    bool collisionTC = collisionDetector.detect(*triangle, triangleTransform, *circle, circleTransform);
    cout << "Do we have a collision between triangle and circle: " << collisionTC << endl;

    // Detect collision between circle and circle, should be a collision
    Penetration penetrationCC;
    bool collisionCC = collisionDetector.detect(*circle, circleTransform, *circle, circleTransform2, penetrationCC);
    cout << "Do we have a collision between circle and circle: " << collisionCC;
    cout << ", penetration normal: (" << penetrationCC.normal.x << "," << penetrationCC.normal.y << ")";
    cout << ", penetration depth: " << penetrationCC.depth << endl;


    // Trying to find a bug
    unique_ptr<Polygon> rectangleStatic = Polygon::createRectangle(0.666667f, 29.666668f);
    Transform2 staticTransform;
    staticTransform.translate(0.433333f, 15.000001f);

    unique_ptr<Polygon> rectangleDynamic = Polygon::createRectangle(0.33333333333f, 0.666666666f);
    Transform2 dynamicTransform;
    // dynamicTransform.translate(1.083334f, 6.666667f);
    dynamicTransform.translate(1.102533f, 9.730800f);

    float angle = Vector2Util::findAngle(fvec2(-1.0f, 1.0f), fvec2(0.0f, 1.0f));
    dynamicTransform.rotateR(angle);

    cout << "static body: " + rectangleStatic->toString() + " transform: " + staticTransform.toString() +
            ", dynamicBody: " + rectangleDynamic->toString() + " transform: " + dynamicTransform.toString() << endl;

    Penetration penetrationSD;
    bool collisionSD = collisionDetector.detect(*rectangleStatic, staticTransform, *rectangleDynamic, dynamicTransform, penetrationSD);
    cout << "Do we have a collision between static and dynamic: " << collisionSD;
    cout << ", penetration normal: (" << penetrationSD.normal.x << "," << penetrationSD.normal.y << ")";
    cout << ", penetration depth: " << penetrationSD.depth << endl;
    
    return 0;
}