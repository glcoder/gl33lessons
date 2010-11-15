#ifndef QUAT_H
#define QUAT_H

#include <cmath>

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct mat2;
struct mat3;
struct mat4;

struct quat
{
	union
	{
		struct { float x, y, z, w; };
		float q[4];
	};

	quat() {}

	void set(const float *f)                     { x = f[0];    y = f[1];    z = f[2];    w = f[3];    }
	void set(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }
	void set(const quat &q)                      { x = q.x;     y = q.y;     z = q.z;     w = q.w;     }

	void set(const vec2 &v);
	void set(const vec3 &v);
	void set(const vec4 &v);
	void set(const vec3 &v, float w);
	void set(const mat3 &M);
	void set(const mat4 &M);

	quat(const float *f)                     { set(f);          }
	quat(float x, float y, float z, float w) { set(x, y, z, w); }
	quat(const quat &q)                      { set(q);          }
	quat(const vec2 &v)                      { set(v);          }
	quat(const vec3 &v)                      { set(v);          }
	quat(const vec4 &v)                      { set(v);          }
	quat(const vec3 &v, float w)             { set(v, w);       }
	quat(const mat3 &M)                      { set(M);          }
	quat(const mat4 &M)                      { set(M);          }

	quat& operator=(const quat &q) { set(q); return *this; }
	quat& operator=(const vec2 &v) { set(v); return *this; }
	quat& operator=(const vec3 &v) { set(v); return *this; }
	quat& operator=(const vec4 &v) { set(v); return *this; }
	quat& operator=(const mat3 &M) { set(M); return *this; }
	quat& operator=(const mat4 &M) { set(M); return *this; }

	float operator[](int i)       { return q[i]; }
	float operator[](int i) const { return q[i]; }

	const quat operator-() const { return quat(-x, -y, -z, -w); }

	const quat operator+(const quat &v) const { return quat(x + v.x, y + v.y, z + v.z, w + v.w); }
	const quat operator-(const quat &v) const { return quat(x - v.x, y - v.y, z - v.z, w - v.w); }
	const quat operator*(float f)       const { return quat(x * f,   y * f,   z * f,   w * f);   }
	const quat operator/(float f)       const { return quat(x / f,   y / f,   z / f,   w / f);   }

	const quat operator*(const quat &q) const
	{
		return quat(w * q.x + x * q.w + y * q.z - z * q.y,
		            w * q.y + y * q.w + z * q.x - x * q.z,
		            w * q.z + z * q.w + x * q.y - y * q.x,
		            w * q.w - x * q.x - y * q.y - z * q.z);
	}

	quat& operator+=(const quat &v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	quat& operator-=(const quat &v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	quat& operator*=(float f)       { x *= f;   y *= f;   z *= f;   w *= f;   return *this; }
	quat& operator/=(float f)       { x /= f;   y /= f;   z /= f;   w /= f;   return *this; }

	quat& operator*=(const quat &q)
	{
		set(w * q.x + x * q.w + y * q.z - z * q.y,
		    w * q.y + y * q.w + z * q.x - x * q.z,
		    w * q.z + z * q.w + x * q.y - y * q.x,
		    w * q.w - x * q.x - y * q.y - z * q.z);
		return *this;
	}
};

inline float dot(const quat &q1, const quat &q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

inline float length(const quat &q)
{
	return sqrtf(dot(q,q));
}

inline const quat normalize(const quat &q)
{
	return q / length(q);
}

inline const quat conjugate(const quat &q)
{
	return quat(-q.x, -q.y, -q.z, q.w);
}

#endif /* QUAT_H */
