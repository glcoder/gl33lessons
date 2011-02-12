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
	union
	{
		struct { float r1[3], r2[3], r3[3]; };
		float m[9];
	};

	mat3() {}

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
		m[0] = M[0]; m[1] = M[1]; m[2] = M[2];
		m[3] = M[3]; m[4] = M[4]; m[5] = M[5];
		m[6] = M[6]; m[7] = M[7]; m[8] = M[8];
	}

	void set(const mat2 &M);
	void set(const mat4 &M);
	void set(const quat &q);

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

	mat3& operator=(const mat3 &M) { set(M); return *this; }
	mat3& operator=(const mat2 &M) { set(M); return *this; }
	mat3& operator=(const mat4 &M) { set(M); return *this; }
	mat3& operator=(const quat &q) { set(q); return *this; }

	float operator[](int i)       { return m[i]; }
	float operator[](int i) const { return m[i]; }

	const mat3 operator+(const mat3 &M) const
	{
		return mat3(m[0] + M[0], m[1] + M[1], m[2] + M[2],
		            m[3] + M[3], m[4] + M[4], m[5] + M[5],
		            m[6] + M[6], m[7] + M[7], m[8] + M[8]);
	}
	const mat3 operator-(const mat3 &M) const
	{
		return mat3(m[0] - M[0], m[1] - M[1], m[2] - M[2],
		            m[3] - M[3], m[4] - M[4], m[5] - M[5],
		            m[6] - M[6], m[7] - M[7], m[8] - M[8]);
	}
	const mat3 operator*(const mat3 &M) const
	{
		return mat3(m[0] * M[0] + m[1] * M[3] + m[2] * M[6],
		            m[0] * M[1] + m[1] * M[4] + m[2] * M[7],
		            m[0] * M[2] + m[1] * M[5] + m[2] * M[8],
		            m[3] * M[0] + m[4] * M[3] + m[5] * M[6],
		            m[3] * M[1] + m[4] * M[4] + m[5] * M[7],
		            m[3] * M[2] + m[4] * M[5] + m[5] * M[8],
		            m[6] * M[0] + m[7] * M[3] + m[8] * M[6],
		            m[6] * M[1] + m[7] * M[4] + m[8] * M[7],
		            m[6] * M[2] + m[7] * M[5] + m[8] * M[8]);
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
		m[0] += M[0]; m[1] += M[1]; m[2] += M[2];
		m[3] += M[3]; m[4] += M[4]; m[5] += M[5];
		m[6] += M[6]; m[7] += M[7]; m[8] += M[8];
		return *this;
	}
	mat3& operator-=(const mat3 &M)
	{
		m[0] -= M[0]; m[1] -= M[1]; m[2] -= M[2];
		m[3] -= M[3]; m[4] -= M[4]; m[5] -= M[5];
		m[6] -= M[6]; m[7] -= M[7]; m[8] -= M[8];
		return *this;
	}
	mat3& operator*=(const mat3 &M)
	{
		set(m[0] * M[0] + m[1] * M[3] + m[2] * M[6],
		    m[0] * M[1] + m[1] * M[4] + m[2] * M[7],
		    m[0] * M[2] + m[1] * M[5] + m[2] * M[8],
		    m[3] * M[0] + m[4] * M[3] + m[5] * M[6],
		    m[3] * M[1] + m[4] * M[4] + m[5] * M[7],
		    m[3] * M[2] + m[4] * M[5] + m[5] * M[8],
		    m[6] * M[0] + m[7] * M[3] + m[8] * M[6],
		    m[6] * M[1] + m[7] * M[4] + m[8] * M[7],
		    m[6] * M[2] + m[7] * M[5] + m[8] * M[8]);
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
	return M[0] * (M[4] * M[8] - M[5] * M[7])
	     - M[1] * (M[3] * M[8] - M[5] * M[6])
	     + M[2] * (M[3] * M[7] - M[4] * M[6]);
}

inline const mat3 inverse(const mat3 &M)
{
	const float det = 1.0f / determinant(M);

	return mat3((M[4] * M[8] - M[5] * M[7]) * det,
	            (M[2] * M[7] - M[1] * M[8]) * det,
	            (M[1] * M[5] - M[2] * M[4]) * det,
	            (M[5] * M[6] - M[3] * M[8]) * det,
	            (M[0] * M[8] - M[2] * M[6]) * det,
	            (M[2] * M[3] - M[0] * M[5]) * det,
	            (M[3] * M[7] - M[4] * M[6]) * det,
	            (M[1] * M[6] - M[0] * M[7]) * det,
	            (M[0] * M[4] - M[1] * M[3]) * det);
}

inline const mat3 transpose(const mat3 &M)
{
	return mat3(M[0], M[3], M[6],
	            M[1], M[4], M[7],
	            M[2], M[5], M[8]);
}

#endif /* MAT3_H */
