#ifndef VEC4_H
#define VEC4_H

#include <cmath>

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct mat2;
struct mat3;
struct mat4;

struct vec4
{
	union
	{
		struct { float x, y, z, w; };
		float v[4];
	};

	vec4() {}

	void set(const float *f)                     { x = f[0];    y = f[1];    z = f[2];    w = f[3];    }
	void set(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }
	void set(const vec4 &v)                      { x = v.x;     y = v.y;     z = v.z;     w = v.w;     }

	void set(const vec2 &v);
	void set(const vec3 &v);

	vec4(const float *f)                     { set(f);          }
	vec4(float x, float y, float z, float w) { set(x, y, z, w); }
	vec4(const vec4 &v)                      { set(v);          }
	vec4(const vec2 &v)                      { set(v);          }
	vec4(const vec3 &v)                      { set(v);          }

	vec4& operator=(const vec4 &v) { set(v); return *this; }
	vec4& operator=(const vec2 &v) { set(v); return *this; }
	vec4& operator=(const vec3 &v) { set(v); return *this; }

	float operator[](int i)       { return v[i]; }
	float operator[](int i) const { return v[i]; }

	const vec4 operator-() const { return vec4(-x, -y, -z, -w); }

	const vec4 operator+(const vec4 &v) const { return vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
	const vec4 operator-(const vec4 &v) const { return vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
	const vec4 operator*(float f)       const { return vec4(x * f,   y * f,   z * f,   w * f);   }
	const vec4 operator/(float f)       const { return vec4(x / f,   y / f,   z / f,   w / f);   }

	vec4& operator+=(const vec4 &v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	vec4& operator-=(const vec4 &v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	vec4& operator*=(float f)       { x *= f;   y *= f;   z *= f;   w *= f;   return *this; }
	vec4& operator/=(float f)       { x /= f;   y /= f;   z /= f;   w /= f;   return *this; }
};

inline float dot(const vec4& v1, const vec4 &v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline float length(const vec4 &v)
{
	return sqrtf(dot(v,v));
}

inline float distance(const vec4 &v1, const vec4 &v2)
{
	return length(v1 - v2);
}

inline const vec4 normalize(const vec4 &v)
{
	return v / length(v);
}

#endif /* VEC4_H */
