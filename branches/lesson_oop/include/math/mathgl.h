#ifndef MATHGL_H
#define MATHGL_H

#include "math3d.h"

namespace GL
{

inline const mat4 RotationX(float angle)
{
	const float c = cosf(angle * math_radians), s = sinf(angle * math_radians);

	return mat4(1, 0, 0, 0,
	            0, c,-s, 0,
	            0, s, c, 0,
	            0, 0, 0, 1);
}

inline const mat4 RotationY(float angle)
{
	const float c = cosf(angle * math_radians), s = sinf(angle * math_radians);

	return mat4(c, 0, s, 0,
	            0, 1, 0, 0,
	           -s, 0, c, 0,
	            0, 0, 0, 1);
}

inline const mat4 RotationZ(float angle)
{
	const float c = cosf(angle * math_radians), s = sinf(angle * math_radians);

	return mat4(c,-s, 0, 0,
	            s, c, 0, 0,
	            0, 0, 1, 0,
	            0, 0, 0, 1);
}

inline const mat4 FromEuler(float x, float y, float z)
{
	const float cx = cosf(x * math_radians), sx = sinf(x * math_radians),
	            cy = cosf(y * math_radians), sy = sinf(y * math_radians),
	            cz = cosf(z * math_radians), sz = sinf(z * math_radians);

	// rotationX * rotationY * rotationZ
	return mat4(cy * cz, -cy * sz, sy, 0,
	            cx * sz + sx * cz * sy, cx * cz - sx * sy * sz, -cy * sx, 0,
	            sx * sz - cx * cz * sy, cz * sx + cx * sy * sz, cx * cy, 0,
	            0, 0, 0, 1);
}

inline const mat4 FromEuler(const vec3 &r)
{
	return FromEuler(r.x, r.y, r.z);
}

inline const vec3 ToEuler(const mat4 &M)
{
	vec3 angle;
	float x, y, C;

	angle.y = asinf(M[2]);
	C       = cosf(angle.y);
	angle.y *= math_degrees;

	if (fabsf(C) > math_epsilon)
	{
		x       =  M[10] / C;
		y       = -M[ 6] / C;
		angle.x = atan2f(y, x) * math_degrees;
		x       =  M[ 0] / C;
		y       = -M[ 1] / C;
		angle.z = atan2f(y, x) * math_degrees;
	} else
	{
		angle.x = 0.0f;
		x       =  M[ 5];
		y       =  M[ 4];
		angle.z = atan2f(y, x) * math_degrees;
	}

	if (angle.x < 0) angle.x += 360;
	if (angle.y < 0) angle.y += 360;
	if (angle.z < 0) angle.z += 360;

	return angle;
}

inline const mat4 Scale(float x, float y, float z)
{
	return mat4(x, 0, 0, 0,
	            0, y, 0, 0,
	            0, 0, z, 0,
	            0, 0, 0, 1);
}

inline const mat4 Scale(const vec3 &s)
{
	return Scale(s.x, s.y, s.z);
}

inline const mat4 Translation(float x, float y, float z)
{
	return mat4(1, 0, 0, x,
	            0, 1, 0, y,
	            0, 0, 1, z,
	            0, 0, 0, 1);
}

inline const mat4 Translation(const vec3 &t)
{
	return Translation(t.x, t.y, t.z);
}

// projection
inline const mat4 Orthographic(float left, float right,
	float bottom, float top, float zNear, float zFar)
{
	const float tx = - (right + left) / (right - left),
	            ty = - (top + bottom) / (top - bottom),
	            tz = - (zFar + zNear) / (zFar - zNear);

	return mat4(2 / (right - left), 0, 0, tx,
	            0, 2 / (top - bottom), 0, ty,
	            0, 0, -2 / (zFar - zNear), tz,
	            0, 0, 0, 1);
}

inline const mat4 Perspective(float fov, float aspect, float zNear, float zFar)
{
	const float f = 1 / tanf(fov * math_radians / 2),
	            A = (zFar + zNear) / (zNear - zFar),
	            B = (2 * zFar * zNear) / (zNear - zFar);

	return mat4(f / aspect, 0, 0, 0,
	            0, f, 0, 0,
	            0, 0, A, B,
	            0, 0, -1, 0);
}

inline const mat4 Frustum(float left, float right,
	float bottom, float top, float zNear, float zFar)
{
	return mat4((2 * zNear) / (right - left), 0, (right + left) / (right - left), 0,
	            0, (2 * zNear) / (top - bottom), (top + bottom) / (top - bottom), 0,
	            0, 0, -(zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
	            0, 0, -1, 0);
}

// view mtrix
inline const mat4 LookAt(const vec3 &position, const vec3 &center, const vec3 &up)
{
	const vec3 f = normalize(position - center);
	const vec3 s = normalize(cross(up, f));
	const vec3 u = normalize(cross(f, s));

	return mat4(s.x, s.y, s.z, -dot(s, position),
	            u.x, u.y, u.z, -dot(u, position),
	            f.x, f.y, f.z, -dot(f, position),
	            0, 0, 0, 1);
}

}

#endif /* MATHGL_H */
