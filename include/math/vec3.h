#ifndef VEC3_H
#define VEC3_H

#include <cmath>

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct mat2;
struct mat3;
struct mat4;

struct vec3
{
	union
	{
		struct { float x, y, z; };
		float v[3];
	};

	vec3() {}

	void set(const float *f)            { x = f[0];    y = f[1];    z = f[2];    }
	void set(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	void set(const vec3 &v)             { x = v.x;     y = v.y;     z = v.z;     }

	void set(const vec2 &v);
	void set(const vec4 &v);

	vec3(const float *f)            { set(f);       }
	vec3(float x, float y, float z) { set(x, y, z); }
	vec3(const vec3 &v)             { set(v);       }
	vec3(const vec2 &v)             { set(v);       }
	vec3(const vec4 &v)             { set(v);       }

	vec3& operator=(const vec3 &v) { set(v); return *this; }
	vec3& operator=(const vec2 &v) { set(v); return *this; }
	vec3& operator=(const vec4 &v) { set(v); return *this; }

	float operator[](int i)       { return v[i]; }
	float operator[](int i) const { return v[i]; }

	const vec3 operator-() const { return vec3(-x, -y, -z); }

	const vec3 operator+(const vec3 &v) const { return vec3(x + v.x, y + v.y, z + v.z); }
	const vec3 operator-(const vec3 &v) const { return vec3(x - v.x, y - v.y, z - v.z); }
	const vec3 operator*(float f)       const { return vec3(x * f,   y * f,   z * f);   }
	const vec3 operator/(float f)       const { return vec3(x / f,   y / f,   z / f);   }

	vec3& operator+=(const vec3 &v) { x += v.x; y += v.y; z += v.z; return *this; }
	vec3& operator-=(const vec3 &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	vec3& operator*=(float f)       { x *= f;   y *= f;   z *= f;   return *this; }
	vec3& operator/=(float f)       { x /= f;   y /= f;   z /= f;   return *this; }
};

inline float dot(const vec3& v1, const vec3 &v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline const vec3 cross(const vec3 &v1, const vec3 &v2)
{
	return vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

inline float length(const vec3 &v)
{
	return sqrtf(dot(v,v));
}

inline float distance(const vec3 &v1, const vec3 &v2)
{
	return length(v1 - v2);
}

inline const vec3 normalize(const vec3 &v)
{
	return v / length(v);
}

#endif /* VEC3_H */
