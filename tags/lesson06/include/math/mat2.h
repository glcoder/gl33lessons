#ifndef MAT2_H
#define MAT2_H

#include <cmath>

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct mat2;
struct mat3;
struct mat4;

struct mat2
{
	union
	{
		struct { float r1[2], r2[2]; };
		float m[4];
	};

	mat2() {}

	void set(const float *f)
	{
		m[0] = f[0]; m[1] = f[1];
		m[2] = f[2]; m[3] = f[3];
	}
	void set(float m0, float m1,
	         float m2, float m3)
	{
		m[0] = m0; m[1] = m1;
		m[2] = m2; m[3] = m3;
	}
	void set(const mat2 &M)
	{
		m[0] = M[0]; m[1] = M[1];
		m[2] = M[2]; m[3] = M[3];
	}

	void set(const mat3 &M);
	void set(const mat4 &M);

	mat2(const float *f) { set(f); }
	mat2(const mat2 &M)  { set(M); }
	mat2(const mat3 &M)  { set(M); }
	mat2(const mat4 &M)  { set(M); }

	mat2(float m0, float m1,
	     float m2, float m3)
	{
		set(m0, m1,
		    m2, m3);
	}

	mat2& operator=(const mat2 &M) { set(M); return *this; }
	mat2& operator=(const mat3 &M) { set(M); return *this; }
	mat2& operator=(const mat4 &M) { set(M); return *this; }

	float operator[](int i)       { return m[i]; }
	float operator[](int i) const { return m[i]; }

	const mat2 operator+(const mat2 &M) const
	{
		return mat2(m[0] + M[0], m[1] + M[1],
		            m[2] + M[2], m[3] + M[3]);
	}
	const mat2 operator-(const mat2 &M) const
	{
		return mat2(m[0] - M[0], m[1] - M[1],
		            m[2] - M[2], m[3] - M[3]);
	}
	const mat2 operator*(const mat2 &M) const
	{
		return mat2(m[0] * M[0] + m[1] * M[2],
		            m[0] * M[1] + m[1] * M[3],
		            m[2] * M[0] + m[3] * M[2],
		            m[2] * M[1] + m[3] * M[3]);
	}
	const mat2 operator*(float f) const
	{
		return mat2(m[0] * f, m[1] * f,
		            m[2] * f, m[3] * f);
	}
	const mat2 operator/(float f) const
	{
		return mat2(m[0] / f, m[1] / f,
		            m[2] / f, m[3] / f);
	}

	mat2& operator+=(const mat2 &M)
	{
		m[0] += M[0]; m[1] += M[1];
		m[2] += M[2]; m[3] += M[3];
		return *this;
	}
	mat2& operator-=(const mat2 &M)
	{
		m[0] -= M[0]; m[1] -= M[1];
		m[2] -= M[2]; m[3] -= M[3];
		return *this;
	}
	mat2& operator*=(const mat2 &M)
	{
		set(m[0] * M[0] + m[1] * M[2],
		    m[0] * M[1] + m[1] * M[3],
		    m[2] * M[0] + m[3] * M[2],
		    m[2] * M[1] + m[3] * M[3]);
		return *this;
	}
	mat2& operator*=(float f)
	{
		m[0] *= f; m[1] *= f;
		m[2] *= f; m[3] *= f;
		return *this;
	}
	mat2& operator/=(float f)
	{
		m[0] /= f; m[1] /= f;
		m[2] /= f; m[3] /= f;
		return *this;
	}
};

inline float determinant(const mat2 &M)
{
	return M[0] * M[3]
	     - M[1] * M[2];
}

inline const mat2 inverse(const mat2 &M)
{
	const float det = 1.0f / determinant(M);
	return mat2(M[3] * det, -M[2] * det,
	           -M[1] * det,  M[0] * det);
}

inline const mat2 transpose(const mat2 &M)
{
	return mat2(M[0], M[2],
	            M[1], M[3]);
}

#endif /* MAT2_H */
