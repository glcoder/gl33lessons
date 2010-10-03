#ifndef MAT3_H
#define MAT3_H

#include <cmath>

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct mat2;
struct mat3;
struct mat4;

struct mat3
{
	float m[9];

	mat3() {}

	void set(float f)
	{
		m[0] = f; m[1] = f; m[2] = f;
		m[3] = f; m[4] = f; m[5] = f;
		m[6] = f; m[7] = f; m[8] = f;
	}
	void set(const float *f)
	{
		m[0] = f[0]; m[1] = f[1]; m[2] = f[2];
		m[3] = f[3]; m[4] = f[4]; m[5] = f[5];
		m[6] = f[6]; m[7] = f[7]; m[8] = f[8];
	}
	void set(float m0, float m1, float m2,
	         float m3, float m4, float m5,
	         float m6, float m7, float m8)
	{
		m[0] = m0; m[1] = m1; m[2] = m2;
		m[3] = m3; m[4] = m4; m[5] = m5;
		m[6] = m6; m[7] = m7; m[8] = m8;
	}
	void set(const mat3 &M)
	{
		m[0] = M.m[0]; m[1] = M.m[1]; m[2] = M.m[2];
		m[3] = M.m[3]; m[4] = M.m[4]; m[5] = M.m[5];
		m[6] = M.m[6]; m[7] = M.m[7]; m[8] = M.m[8];
	}

	void set(const mat2 &M);
	void set(const mat4 &M);
	void set(const quat &q);

	mat3(float f)        { set(f); }
	mat3(const float *f) { set(f); }
	mat3(const mat3 &M)  { set(M); }
	mat3(const mat2 &M)  { set(M); }
	mat3(const mat4 &M)  { set(M); }
	mat3(const quat &q)  { set(q); }

	mat3(float m0, float m1, float m2,
	     float m3, float m4, float m5,
	     float m6, float m7, float m8)
	{
		set(m0, m1, m2,
		    m3, m4, m5,
		    m6, m7, m8);
	}

	mat3& operator=(float f)       { set(f); return *this; }
	mat3& operator=(const mat3 &M) { set(M); return *this; }
	mat3& operator=(const mat2 &M) { set(M); return *this; }
	mat3& operator=(const mat4 &M) { set(M); return *this; }
	mat3& operator=(const quat &q) { set(q); return *this; }

	float operator[](int i)       { return m[i]; }
	float operator[](int i) const { return m[i]; }

	const mat3 operator+(const mat3 &M) const
	{
		return mat3(m[0] + M.m[0], m[1] + M.m[1], m[2] + M.m[2],
		            m[3] + M.m[3], m[4] + M.m[4], m[5] + M.m[5],
		            m[6] + M.m[6], m[7] + M.m[7], m[8] + M.m[8]);
	}
	const mat3 operator-(const mat3 &M) const
	{
		return mat3(m[0] - M.m[0], m[1] - M.m[1], m[2] - M.m[2],
		            m[3] - M.m[3], m[4] - M.m[4], m[5] - M.m[5],
		            m[6] - M.m[6], m[7] - M.m[7], m[8] - M.m[8]);
	}
	const mat3 operator*(const mat3 &M) const
	{
		return mat3(m[0] * M.m[0] + m[1] * M.m[3] + m[2] * M.m[6],
		            m[0] * M.m[1] + m[1] * M.m[4] + m[2] * M.m[7],
		            m[0] * M.m[2] + m[1] * M.m[5] + m[2] * M.m[8],
		            m[3] * M.m[0] + m[4] * M.m[3] + m[5] * M.m[6],
		            m[3] * M.m[1] + m[4] * M.m[4] + m[5] * M.m[7],
		            m[3] * M.m[2] + m[4] * M.m[5] + m[5] * M.m[8],
		            m[6] * M.m[0] + m[7] * M.m[3] + m[8] * M.m[6],
		            m[6] * M.m[1] + m[7] * M.m[4] + m[8] * M.m[7],
		            m[6] * M.m[2] + m[7] * M.m[5] + m[8] * M.m[8]);
	}
	const mat3 operator*(float f) const
	{
		return mat3(m[0] * f, m[1] * f, m[2] * f,
		            m[3] * f, m[4] * f, m[5] * f,
		            m[6] * f, m[7] * f, m[8] * f);
	}
	const mat3 operator/(float f) const
	{
		return mat3(m[0] / f, m[1] / f, m[2] / f,
		            m[3] / f, m[4] / f, m[5] / f,
		            m[6] / f, m[7] / f, m[8] / f);
	}

	mat3& operator+=(const mat3 &M)
	{
		m[0] += M.m[0]; m[1] += M.m[1]; m[2] += M.m[2];
		m[3] += M.m[3]; m[4] += M.m[4]; m[5] += M.m[5];
		m[6] += M.m[6]; m[7] += M.m[7]; m[8] += M.m[8];
		return *this;
	}
	mat3& operator-=(const mat3 &M)
	{
		m[0] -= M.m[0]; m[1] -= M.m[1]; m[2] -= M.m[2];
		m[3] -= M.m[3]; m[4] -= M.m[4]; m[5] -= M.m[5];
		m[6] -= M.m[6]; m[7] -= M.m[7]; m[8] -= M.m[8];
		return *this;
	}
	mat3& operator*=(const mat3 &M)
	{
		set(m[0] * M.m[0] + m[1] * M.m[3] + m[2] * M.m[6],
		    m[0] * M.m[1] + m[1] * M.m[4] + m[2] * M.m[7],
		    m[0] * M.m[2] + m[1] * M.m[5] + m[2] * M.m[8],
		    m[3] * M.m[0] + m[4] * M.m[3] + m[5] * M.m[6],
		    m[3] * M.m[1] + m[4] * M.m[4] + m[5] * M.m[7],
		    m[3] * M.m[2] + m[4] * M.m[5] + m[5] * M.m[8],
		    m[6] * M.m[0] + m[7] * M.m[3] + m[8] * M.m[6],
		    m[6] * M.m[1] + m[7] * M.m[4] + m[8] * M.m[7],
		    m[6] * M.m[2] + m[7] * M.m[5] + m[8] * M.m[8]);
		return *this;
	}
	mat3& operator*=(float f)
	{
		m[0] *= f; m[1] *= f; m[2] *= f;
		m[3] *= f; m[4] *= f; m[5] *= f;
		m[6] *= f; m[7] *= f; m[8] *= f;
		return *this;
	}
	mat3& operator/=(float f)
	{
		m[0] /= f; m[1] /= f; m[2] /= f;
		m[3] /= f; m[4] /= f; m[5] /= f;
		m[6] /= f; m[7] /= f; m[8] /= f;
		return *this;
	}
};

inline float determinant(const mat3 &M)
{
	return M.m[0] * (M.m[4] * M.m[8] - M.m[5] * M.m[7])
	     - M.m[1] * (M.m[3] * M.m[8] - M.m[5] * M.m[6])
	     + M.m[2] * (M.m[3] * M.m[7] - M.m[4] * M.m[6]);
}

inline const mat3 inverse(const mat3 &M)
{
	const float det = 1.0f / determinant(M);

	return mat3((M.m[4] * M.m[8] - M.m[5] * M.m[7]) * det,
	            (M.m[2] * M.m[7] - M.m[1] * M.m[8]) * det,
	            (M.m[1] * M.m[5] - M.m[2] * M.m[4]) * det,
	            (M.m[5] * M.m[6] - M.m[3] * M.m[8]) * det,
	            (M.m[0] * M.m[8] - M.m[2] * M.m[6]) * det,
	            (M.m[2] * M.m[3] - M.m[0] * M.m[5]) * det,
	            (M.m[3] * M.m[7] - M.m[4] * M.m[6]) * det,
	            (M.m[1] * M.m[6] - M.m[0] * M.m[7]) * det,
	            (M.m[0] * M.m[4] - M.m[1] * M.m[3]) * det);
}

inline const mat3 transpose(const mat3 &M)
{
	return mat3(M.m[0], M.m[3], M.m[6],
	            M.m[1], M.m[4], M.m[7],
	            M.m[2], M.m[5], M.m[8]);
}

#endif /* MAT3_H */
