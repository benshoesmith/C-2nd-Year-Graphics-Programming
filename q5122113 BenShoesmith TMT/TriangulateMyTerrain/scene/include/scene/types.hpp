#pragma once

#include "config.hpp"

namespace scene {

/**
* A generic unrecoverable error exception
*/
class SCENE_API Exception
{
public:
    virtual const char * what() noexcept
    {
        return "an unknown exception has occurred";
    }
};


/**
 * A utility class to specify 2d vectors.
 */
class SCENE_API Vector2
{
public:

    float x, y;

public:

    Vector2() : x(0), y(0) {}

    Vector2(float X, float Y) : x(X), y(Y) {}

};


/**
 * A utility class to specify 3d vectors.
 */
class SCENE_API Vector3
{
public:

    float x, y, z;

public:

    Vector3() : x(0), y(0), z(0) {}

    Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vector3(const Vector2& v, float Z) : x(v.x), y(v.y), z(Z) {}

};


/**
 * A utility class to specify 4d vectors.
 */
class SCENE_API Vector4
{
public:

    float x, y, z, w;

public:

    Vector4() : x(0), y(0), z(0), w(1) {}

    Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

    Vector4(const Vector3& v, float W) : x(v.x), y(v.y), z(v.z), w(W) {}

};


/**
 * A utility class to specify 4x4 matrices.
 */
class SCENE_API Matrix4x4
{
public:

    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;

public:

    Matrix4x4() :
        m00(1), m10(0), m20(0), m30(0),
        m01(0), m11(1), m21(0), m31(0),
        m02(0), m12(0), m22(1), m32(0),
        m03(0), m13(0), m23(0), m33(1)
    {
    }

    Matrix4x4(float M00, float M01, float M02, float M03,
              float M10, float M11, float M12, float M13,
              float M20, float M21, float M22, float M23,
              float M30, float M31, float M32, float M33) :
        m00(M00), m10(M10), m20(M20), m30(M30),
        m01(M01), m11(M11), m21(M21), m31(M31),
        m02(M02), m12(M12), m22(M22), m32(M32),
        m03(M03), m13(M13), m23(M23), m33(M33)
    {
    }

};


/**
* A utility class to specify 4x3 matrices.
*/
class SCENE_API Matrix4x3
{
public:

    float m00, m01, m02;
    float m10, m11, m12;
    float m20, m21, m22;
    float m30, m31, m32;

public:

    Matrix4x3() :
        m00(1), m10(0), m20(0), m30(0),
        m01(0), m11(1), m21(0), m31(0),
        m02(0), m12(0), m22(1), m32(0)
    {
    }

    Matrix4x3(float M00, float M01, float M02,
              float M10, float M11, float M12,
              float M20, float M21, float M22,
              float M30, float M31, float M32) :
        m00(M00), m10(M10), m20(M20), m30(M30),
        m01(M01), m11(M11), m21(M21), m31(M31),
        m02(M02), m12(M12), m22(M22), m32(M32)
    {
    }

};

} // end namespace scene
