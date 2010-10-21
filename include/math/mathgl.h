#ifndef MATHGL_H
#define MATHGL_H

#include "math3d.h"

// transformation
inline const mat4 GLRotationX(float angle)
{
	const float c = cosf(angle * math_radians), s = sinf(angle * math_radians);

	return mat4(1, 0, 0, 0,
	            0, c,-s, 0,
	            0, s, c, 0,
	            0, 0, 0, 1);
}

inline const mat4 GLRotationY(float angle)
{
	const float c = cosf(angle * math_radians), s = sinf(angle * math_radians);

	return mat4(c, 0, s, 0,
	            0, 1, 0, 0,
	           -s, 0, c, 0,
	            0, 0, 0, 1);
}

inline const mat4 GLRotationZ(float angle)
{
	const float c = cosf(angle * math_radians), s = sinf(angle * math_radians);

	return mat4(c,-s, 0, 0,
	            s, c, 0, 0,
	            0, 0, 1, 0,
	            0, 0, 0, 1);
}

inline const mat4 GLRotation(float x, float y, float z)
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

inline const mat4 GLRotation(const vec3 &r)
{
	return GLRotation(r.x, r.y, r.z);
}

inline const mat4 GLScale(float x, float y, float z)
{
	return mat4(x, 0, 0, 0,
	            0, y, 0, 0,
	            0, 0, z, 0,
	            0, 0, 0, 1);
}

inline const mat4 GLScale(const vec3 &s)
{
	return GLScale(s.x, s.y, s.z);
}

inline const mat4 GLTranslation(float x, float y, float z)
{
	return mat4(1, 0, 0, x,
	            0, 1, 0, y,
	            0, 0, 1, z,
	            0, 0, 0, 1);
}

inline const mat4 GLTranslation(const vec3 &t)
{
	return GLTranslation(t.x, t.y, t.z);
}

// projection
inline const mat4 GLOrthographic(float left, float right,
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

inline const mat4 GLPerspective(float fov, float aspect, float zNear, float zFar)
{
	const float f = 1 / tanf(fov * math_radians / 2),
	            A = (zFar + zNear) / (zNear - zFar),
	            B = (2 * zFar * zNear) / (zNear - zFar);

	return mat4(f / aspect, 0, 0, 0,
	            0, f, 0, 0,
	            0, 0, A, B,
	            0, 0, -1, 0);
}

inline const mat4 GLFrustum(float left, float right,
	float bottom, float top, float zNear, float zFar)
{
	return mat4((2 * zNear) / (right - left), 0, (right + left) / (right - left), 0,
	            0, (2 * zNear) / (top - bottom), (top + bottom) / (top - bottom), 0,
	            0, 0, -(zFar + zNear) / (zFar - zNear), -(2 * zFar * zNear) / (zFar - zNear),
	            0, 0, -1, 0);
}

// view mtrix
inline const mat4 GLLookAt(const vec3 &position, const vec3 &center, const vec3 &up)
{
	const vec3 f = normalize(position - center);
	const vec3 s = normalize(cross(up, f));
	const vec3 u = normalize(cross(f, s));

	return mat4(s.x, s.y, s.z, -dot(s, position),
	            u.x, u.y, u.z, -dot(u, position),
	            f.x, f.y, f.z, -dot(f, position),
	            0, 0, 0, 1);
}

#endif /* MATHGL_H */
