#ifndef MATH3D_H
#define MATH3D_H

/* TODO:
1. Check quaternion conversion to and from matrix
2. Add quaternion-related functions
3. Check for matrix transformation functions
*/

#include <cmath>

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "mat2.h"
#include "mat3.h"
#include "mat4.h"

// math constants
const float math_epsilon = 1e-5f;

const float math_radians = 3.14159265f / 180.0f;
const float math_degrees = 180.0f / 3.14159265f;

// vec2
inline void vec2::set(const vec3 &v) { x = v.x; y = v.y; }
inline void vec2::set(const vec4 &v) { x = v.x; y = v.y; }

// vec3
inline void vec3::set(const vec2 &v) { x = v.x; y = v.y; z = 0;   }
inline void vec3::set(const vec4 &v) { x = v.x; y = v.y; z = v.z; }

// vec4
inline void vec4::set(const vec2 &v) { x = v.x; y = v.y; z = 0;   w = 0; }
inline void vec4::set(const vec3 &v) { x = v.x; y = v.y; z = v.z; w = 0; }

// quat
inline void quat::set(const vec2 &v)          { x = v.x; y = v.y; z = 0;   w = 1;       }
inline void quat::set(const vec3 &v)          { x = v.x; y = v.y; z = v.z; w = 1;       }
inline void quat::set(const vec4 &v)          { x = v.x; y = v.y; z = v.z; w = v.w;     }
inline void quat::set(const vec3 &v, float w) { x = v.x; y = v.y; z = v.z; this->w = w; }

inline void quat::set(const mat3 &M)
{
	const float t = M.m[0] + M.m[4] + M.m[8] + 1;
	float       s;

	if (t > math_epsilon)
	{
		s = sqrtf(t) * 2;
		x = (M.m[7] - M.m[5]) / s;
		y = (M.m[2] - M.m[6]) / s;
		z = (M.m[3] - M.m[1]) / s;
		w = 0.25f * s;
	} else if (M.m[0] > M.m[4] && M.m[0] > M.m[8])
	{
		s = sqrtf(1 + M.m[0] - M.m[4] - M.m[8]) * 2;
		x = 0.25f * s;
		y = (M.m[3] + M.m[1]) / s;
		z = (M.m[2] + M.m[6]) / s;
		w = (M.m[7] - M.m[5]) / s;
	} else if (M.m[4] > M.m[8])
	{
		s = sqrtf(1 + M.m[4] - M.m[0] - M.m[8]) * 2;
		x = (M.m[3] + M.m[1]) / s;
		y = 0.25f * s;
		z = (M.m[7] + M.m[5]) / s;
		w = (M.m[2] - M.m[6]) / s;
	} else
	{
		s = sqrtf(1 + M.m[8] - M.m[0] - M.m[4]) * 2;
		x = (M.m[2] + M.m[6]) / s;
		y = (M.m[7] + M.m[5]) / s;
		z = 0.25f * s;
		w = (M.m[4] - M.m[1]) / s;
	}

	*this = normalize(*this);
}

inline void quat::set(const mat4 &M)
{
	set(mat3(M));
}

// mat2
inline void mat2::set(const mat3 &M)
{
	set(M.m[0], M.m[1],
	    M.m[3], M.m[4]);
}

inline void mat2::set(const mat4 &M)
{
	set(M.m[0], M.m[1],
	    M.m[4], M.m[5]);
}

inline const vec2 operator*(const mat2 &M, const vec2 &v)
{
	return vec2(M.m[0] * v.x + M.m[1] * v.y,
	            M.m[2] * v.x + M.m[3] * v.y);
}

// mat3
inline void mat3::set(const mat2 &M)
{
	set(M.m[0], M.m[1], 0,
	    M.m[2], M.m[3], 0,
	    0,      0,      1);
}

inline void mat3::set(const mat4 &M)
{
	set(M.m[0], M.m[1], M.m[2],
	    M.m[4], M.m[5], M.m[6],
	    M.m[8], M.m[9], M.m[10]);
}

inline void mat3::set(const quat &q)
{
	const float xx = q.x * q.x,
	            xy = q.x * q.y,
	            xz = q.x * q.z,
	            xw = q.x * q.w,
	            yy = q.y * q.y,
	            yz = q.y * q.z,
	            yw = q.y * q.w,
	            zz = q.z * q.z,
	            zw = q.z * q.w;

	set(1 - 2 * (yy + zz), 2 * (xy + zw), 2 * (xz - yw),
	    2 * (xy - zw), 1 - 2 * (xx + zz), 2 * (yz + xw),
	    2 * (xz + yw), 2 * (yz - xw), 1 - 2 * (xx + yy));
}

inline const vec2 operator*(const mat3 &M, const vec2 &v)
{
	return vec2(M.m[0] * v.x + M.m[1] * v.y + M.m[2],
	            M.m[3] * v.x + M.m[4] * v.y + M.m[5]);
}

inline const vec3 operator*(const mat3 &M, const vec3 &v)
{
	return vec3(M.m[0] * v.x + M.m[1] * v.y + M.m[2] * v.z,
	            M.m[3] * v.x + M.m[4] * v.y + M.m[5] * v.z,
	            M.m[6] * v.x + M.m[7] * v.y + M.m[8] * v.z);
}

// mat4
inline void mat4::set(const mat2 &M)
{
	set(M.m[0], M.m[1], 0, 0,
	    M.m[2], M.m[3], 0, 0,
	    0,      0,      1, 0,
	    0,      0,      0, 1);
}

inline void mat4::set(const mat3 &M)
{
	set(M.m[0], M.m[1], M.m[2], 0,
	    M.m[3], M.m[4], M.m[5], 0,
	    M.m[6], M.m[7], M.m[8], 0,
	    0,      0,      0,      1);
}

inline void mat4::set(const quat &q)
{
	set(mat3(q));
}

inline const vec2 operator*(const mat4 &M, const vec2 &v)
{
	return vec2(M.m[0] * v.x + M.m[1] * v.y + M.m[2] + M.m[3],
	            M.m[4] * v.x + M.m[5] * v.y + M.m[6] + M.m[7]);
}

inline const vec3 operator*(const mat4 &M, const vec3 &v)
{
	return vec3(M.m[ 0] * v.x + M.m[ 1] * v.y + M.m[ 2] * v.z + M.m[ 3],
	            M.m[ 4] * v.x + M.m[ 5] * v.y + M.m[ 6] * v.z + M.m[ 7],
	            M.m[ 8] * v.x + M.m[ 9] * v.y + M.m[10] * v.z + M.m[11]);
}

inline const vec4 operator*(const mat4 &M, const vec4 &v)
{
	return vec4(M.m[ 0] * v.x + M.m[ 1] * v.y + M.m[ 2] * v.z + M.m[ 3] * v.w,
	            M.m[ 4] * v.x + M.m[ 5] * v.y + M.m[ 6] * v.z + M.m[ 7] * v.w,
	            M.m[ 8] * v.x + M.m[ 9] * v.y + M.m[10] * v.z + M.m[11] * v.w,
	            M.m[12] * v.x + M.m[13] * v.y + M.m[14] * v.z + M.m[15] * v.w);
}

// transformation
inline const mat4 rotation(float x, float y, float z)
{
	const float A = cosf(x), B = sinf(x), C = cosf(y),
	            D = sinf(y), E = cosf(z), F = sinf(z);
	const float AD = A * D, BD = B * D;

	return mat4(C * E, -C * F, D, 0,
	            BD * E + A * F, -BD * F + A * E, -B * C, 0,
	           -AD * E + B * F, AD * F + B * E, A * C, 0,
	            0, 0, 0, 1);
}

inline const mat4 translation(float x, float y, float z)
{
	return mat4(1, 0, 0, x,
	            0, 1, 0, y,
	            0, 0, 1, z,
	            0, 0, 0, 1);
}

// projection
inline const mat4 orthographic(float left, float right,
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

inline const mat4 perspective(float fov, float aspect, float zNear, float zFar)
{
	const float f = 1 / tanf(fov / 2),
	            A = (zFar + zNear) / (zNear - zFar),
	            B = (2 * zFar * zNear) / (zNear - zFar);

	return mat4(f / aspect, 0, 0, 0,
	            0, f, 0, 0,
	            0, 0, A, B,
	            0, 0, -1, 0);
}

// view mtrix
inline const mat4 lookat(const vec3 &position, const vec3 &center, const vec3 &up)
{
	const vec3 f = normalize(position - center);
	const vec3 s = normalize(cross(up, f));
	const vec3 u = cross(f, s);

	return mat4(s.x, s.y, s.z, -dot(s, position),
	            u.x, u.y, u.z, -dot(u, position),
	            f.x, f.y, f.z, -dot(f, position),
	            0, 0, 0, 1);
}

#endif /* MATH3D_H */
