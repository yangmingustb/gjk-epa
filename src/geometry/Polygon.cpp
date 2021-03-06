#include <geometry/Polygon.h>
#include <math/Vector2Util.h>
#include <glm/geometric.hpp>
#include <cassert>
#include <cmath>

using glm::cross;
using glm::dot;

const float INV_3 = 1.0 / 3.0;

Polygon::Polygon(const ConvexType convextType, std::initializer_list<fvec2> points)
    : Convex(convextType)
{
    vertices.insert(vertices.end(), points.begin(), points.end());

    assert(valid());

    center = calcAreaWeightedCenter(vertices);
}

Polygon::Polygon(const ConvexType convexType, fvec2 center, std::initializer_list<fvec2> points)
    : Convex(convexType)
{
    vertices.insert(vertices.end(), points.begin(), points.end());

    assert(valid());

    this->center = center;
}

unique_ptr<Polygon> Polygon::createPolygon(std::initializer_list<fvec2> points)
{
    return unique_ptr<Polygon>(new Polygon(ConvexType::POLYGON, points));
}

unique_ptr<Polygon> Polygon::createTriangle(fvec2 point1, fvec2 point2, fvec2 point3)
{
    return unique_ptr<Polygon>(new Polygon(ConvexType::TRIANGLE, {point1, point2, point3}));
}

unique_ptr<Polygon> Polygon::createRectangle(float width, float height)
{
    assert(width > 0.0 && height > 0.0);

    fvec2 point1(-width * 0.5, -height *0.5);
    fvec2 point2(width * 0.5,  -height *0.5);
    fvec2 point3(width * 0.5,   height *0.5);
    fvec2 point4(-width * 0.5,  height *0.5);

    return unique_ptr<Polygon>(new Polygon(ConvexType::RECTANGLE, fvec2(), {point1, point2, point3, point4}));
}

const fvec2& Polygon::getCenter() const
{
    return center;
}

const fvec2 Polygon::getFarthestPoint(const fvec2 direction, const Transform2& transform) const
{
    // transform the normal into local space
    fvec2 localn = transform.getInverseTransformedR(direction);

    // set the farthest point to the first one
    fvec2 point = vertices[0];

    // prime the projection amount
    float max = dot(localn, vertices[0]);

    // loop through the rest of the vertices to find a further point along the axis
    size_t size = vertices.size();
    for (uint32_t i = 1; i < size; i++)
    {
        // get the current vertex
        fvec2 v = vertices[i];

        // project the vertex onto the axis
        float projection = dot(localn, v);

        // check to see if the projection is greater than the last
        if (projection > max)
        {
            // otherwise this point is the farthest so far so clear the array and add it
            point = v;

            // set the new maximum
            max = projection;
        }
    }

    // transform the point into world space
    transform.transform(point);

    return point;
}

fvec2 Polygon::calcAreaWeightedCenter(const vector<fvec2>& points)
{
    size_t size = points.size();

    // check for empty list
    if (size <= 0) assert("Empty set of points");

    // check for list of one point
    if (size == 1)
    {
        fvec2 p = points[0];
        return p;
    }

    // get the average center
    fvec2 ac;
    for (uint32_t i = 0; i < size; i++)
    {
        fvec2 p = points[i];
        ac += p;
    }

    ac *= (1.0 / size);

    // otherwise perform the computation
    fvec2 center;
    float area = 0.0;

    // loop through the vertices
    for (uint32_t i = 0; i < size; i++)
    {
        // get two vertices
        fvec2 p1 = points[i];
        fvec2 p2 = i + 1 < size ? points[i + 1] : points[0];
        p1 -= ac;
        p2 -= ac;

        // perform the cross product (yi * x(i+1) - y(i+1) * xi)
        float d = (p1.x * p2.y) - (p1.y * p2.x);

        // multiply by half
        float triangleArea = 0.5 * d;

        // add it to the total area
        area += triangleArea;

        // area weighted centroid
        // (p1 + p2) * (D / 3)
        // = (x1 + x2) * (yi * x(i+1) - y(i+1) * xi) / 3
        // we will divide by the total area later
        center += (p1 + p2) * INV_3 * triangleArea;
    }

    // check for zero area
    if (abs(area) <= std::numeric_limits<float>::epsilon())
    {
        // zero area can only happen if all the points are the same point
        // in which case just return a copy of the first
        return points[0];
    }

    // finish the centroid calculation by dividing by the total area
    center *= (1.0 / area);
    center += ac;

    // return the center
    return center;
}

bool Polygon::valid() const
{
    int32_t size = (int32_t) vertices.size();
    if (size < 3)
    {
        return false;
    }

    // check for convex
    float area = 0.0;
    float sign = 0.0;
    for (int32_t i = 0; i < size; i++)
    {
        fvec2 p0 = (i - 1 < 0) ? vertices[size - 1] : vertices[i - 1];
        fvec2 p1 = vertices[i];
        fvec2 p2 = (i + 1 == size) ? vertices[0] : vertices[i + 1];

        // check for coincident vertices
        if (p1 == p2)
        {
            return false;
        }

        // check the cross product for CCW winding
        float cross = Vector2Util::cross(p1 - p0, p2 - p1);
        float tsign = glm::sign(cross);
        area += cross;

        // check for co-linear points (for now its allowed)
        if (abs(cross) > std::numeric_limits<float>::epsilon())
        {
            // check for convexity
            if (sign != 0.0 && tsign != sign)
            {
                return false;
            }
        }

        sign = tsign;
    }

    // check for CCW
    return area >= 0.0;
}

const string Polygon::toString() const
{
    string verticesS = "[";
    for (auto&& vertex : this->vertices)
    {
        verticesS += glm::to_string(vertex);
    }
    verticesS += "]";

    string polygonType = "RECTANLE";
    if (this->getType() == ConvexType::TRIANGLE)
    {
        polygonType = "TRIANGLE";
    }
    else if (this->getType() == ConvexType::POLYGON)
    {
        polygonType = "POLYGON";
    }

    return "Polygon [" + polygonType + "] - verticies=" + verticesS + ", center=" + glm::to_string(center);
}

