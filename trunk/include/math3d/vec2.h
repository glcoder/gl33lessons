#ifndef VEC2_H
#define VEC2_H

#include <cmath>

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct mat2;
struct mat3;
struct mat4;

struct vec2
{
	union
	{
		struct { float x, y; };
		float v[2];
	};

	vec2() {}

	void set(const float *f)   { x = f[0];    y = f[1];    }
	void set(float x, float y) { this->x = x; this->y = y; }
	void set(const vec2 &v)    { x = v.x;     y = v.y;     }

	void set(const vec3 &v);
	void set(const vec4 &v);

	vec2(const float *f)   { set(f);    }
	vec2(float x, float y) { set(x, y); }
	vec2(const vec2 &v)    { set(v);    }
	vec2(const vec3 &v)    { set(v);    }
	vec2(const vec4 &v)    { set(v);    }

	vec2& operator=(const vec2 &v) { set(v); return *this; }
	vec2& operator=(const vec3 &v) { set(v); return *this; }
	vec2& operator=(const vec4 &v) { set(v); return *this; }

	float operator[](int i)       { return v[i]; }
	float operator[](int i) const { return v[i]; }

	const vec2 operator-() const { return vec2(-x, -y); }

	const vec2 operator+(const vec2 &v) const { return vec2(x + v.x, y + v.y); }
	const vec2 operator-(const vec2 &v) const { return vec2(x - v.x, y - v.y); }
	const vec2 operator*(float f)       const { return vec2(x * f,   y * f);   }
	const vec2 operator/(float f)       const { return vec2(x / f,   y / f);   }

	vec2& operator+=(const vec2 &v) { x += v.x; y += v.y; return *this; }
	vec2& operator-=(const vec2 &v) { x -= v.x; y -= v.y; return *this; }
	vec2& operator*=(float f)       { x *= f;   y *= f;   return *this; }
	vec2& operator/=(float f)       { x /= f;   y /= f;   return *this; }
};

inline float dot(const vec2& v1, const vec2 &v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

/*inline const vec2 cross(const vec2 &v)
{
	return vec2(-v.y, v.x);
}*/

inline float length(const vec2 &v)
{
	return sqrtf(dot(v,v));
}

inline float distance(const vec2 &v1, const vec2 &v2)
{
	return length(v1 - v2);
}

inline const vec2 normalize(const vec2 &v)
{
	return v / length(v);
}

#endif /* VEC2_H */
