#include "Vector3.h"
#include "Sys/Cylinder.h"
#include "Sys/Sphere.h"

namespace DroughtMath
{

inline Vector3f CylinderToCartesian(const f32 r, const f32 theta, const f32 y = 0.0f) {
    return Vector3f(
        r * pikmin2_cosf(theta),
        y, 
        r * pikmin2_sinf(theta)
    );
}


Vector3f RotatePoint(const Vector3f& vec, const f32 theta, const Vector3f& origin) {
    Vector3f originVec = vec - origin;
    return Vector3f(
        pikmin2_cosf(theta) * originVec.x - pikmin2_sinf(theta) * originVec.z + origin.x,
        vec.y,
        pikmin2_sinf(theta) * originVec.x + pikmin2_cosf(theta) * originVec.z + origin.z
    );
}

Vector2f RotatePoint(const Vector2f& vec, const Vector2f& origin, const f32 theta) {
    Vector2f originVec = vec - origin;
    return Vector2f(
        pikmin2_cosf(theta) * originVec.x - pikmin2_sinf(theta) * originVec.y + origin.x,
        pikmin2_sinf(theta) * originVec.x + pikmin2_cosf(theta) * originVec.y + origin.y
    );
}

inline Vector3f getClostestPointOnLine(const Vector3f& point, const Vector3f& lineStart, const Vector3f& lineEnd) {
    Vector3f lineDirection = lineEnd - lineStart;

    Vector3f linestartPointVector = point - lineStart;

    // calculates how far along the line we need to go to get to the clostest point
    f32 projection = dot(lineDirection, linestartPointVector) / lineDirection.sqrMagnitude();

    if (projection > 1.0f) {
        projection = 1.0f;
    }
    else if (projection < 0.0f) {
        projection = 0.0f;
    }

    return lineStart + lineDirection * projection;
}

inline Vector3f getClostestPointOnLine(const Vector3f& point, const Vector3f& lineStart, const Vector3f& lineEnd, f32& outDotProd) {
    Vector3f lineDirection = lineEnd - lineStart;

    Vector3f linestartPointVector = point - lineStart;

    // calculates how far along the line we need to go to get to the clostest point
    outDotProd = dot(lineDirection, linestartPointVector) / lineDirection.sqrMagnitude();

    if (outDotProd > 1.0f) {
        outDotProd = 1.0f;
    }
    else if (outDotProd < 0.0f) {
        outDotProd = 0.0f;
    }

    return lineStart + lineDirection * outDotProd;
}


inline f32 getDistanceToLine(Vector3f& point, const Vector3f& lineStart, const Vector3f& lineEnd) {
    Vector3f clostestPoint = getClostestPointOnLine(point, lineStart, lineEnd);
    return _distanceBetween(clostestPoint, point);
}

inline f32 getDistanceToLine(Vector3f& point, const Vector3f& lineStart, const Vector3f& lineEnd, f32& outDotProd) {
    Vector3f clostestPoint = getClostestPointOnLine(point, lineStart, lineEnd, outDotProd);
    return _distanceBetween(clostestPoint, point);
}

inline f32 getSqrDistanceToLine(Vector3f& point, const Vector3f& lineStart, const Vector3f& lineEnd) {
    Vector3f clostestPoint = getClostestPointOnLine(point, lineStart, lineEnd);
    return sqrDistance(clostestPoint, point);
}


inline f32 getSqrDistanceToLine(Vector3f& point, const Vector3f& lineStart, const Vector3f& lineEnd, f32& outDotProd) {
    Vector3f clostestPoint = getClostestPointOnLine(point, lineStart, lineEnd, outDotProd);
    return sqrDistance(clostestPoint, point);
}

inline Vector3f Lerp(const f32 val, const Vector3f& first, const Vector3f& second) {
    return first * (1.0f - val) + second * val;
}

inline Vector3f getClostestPointOnCylinder(const Vector3f& point, const Sys::Cylinder& cylinder) {
    Vector3f offset = cylinder.mAxis * cylinder.mLength;
    Vector3f pointA = cylinder.mCenter + offset;
    Vector3f pointB = cylinder.mCenter - offset;

    Vector3f clostestOnInnerLine = getClostestPointOnLine(point, pointA, pointB);

    Vector3f delta = clostestOnInnerLine - point;

    delta.normalise();

    return clostestOnInnerLine + delta * cylinder.mRadius;
}

// makes a sphere that contains the two points
inline Sys::Sphere makeBoundingSphere(const Vector3f& pointA, const Vector3f& pointB) {
    Vector3f pos = (pointA + pointB) * 0.5f;
    f32 rad = (pointA - pointB).length() * 0.5f; // distance but I'm const so (:
    return Sys::Sphere(pos, rad);
}

// makes a sphere that contains the two spheres
inline Sys::Sphere makeBoundingSphere(const Sys::Sphere& sphereA, const Sys::Sphere& sphereB) {
    Vector3f differenceVector = sphereA.mPosition - sphereB.mPosition;
    f32 differenceRadius = differenceVector.normalise();
    

    Vector3f pos = (differenceVector * (sphereA.mRadius - sphereB.mRadius) + sphereA.mPosition + sphereB.mPosition) * 0.5f;
    f32 rad = differenceRadius + sphereA.mRadius + sphereB.mRadius;
    return Sys::Sphere(pos, rad);
}

} // namespace DroughtMath
