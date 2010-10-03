#ifndef MAT4_H
#define MAT4_H

#include <cmath>

struct vec2;
struct vec3;
struct vec4;
struct quat;
struct mat2;
struct mat3;
struct mat4;

struct mat4
{
	float m[16];

	mat4() {}

	void set(float f)
	{
		m[ 0] = f; m[ 1] = f; m[ 2] = f; m[ 3] = f;
		m[ 4] = f; m[ 5] = f; m[ 6] = f; m[ 7] = f;
		m[ 8] = f; m[ 9] = f; m[10] = f; m[11] = f;
		m[12] = f; m[13] = f; m[14] = f; m[15] = f;
	}
	void set(const float *f)
	{
		m[ 0] = f[ 0]; m[ 1] = f[ 1]; m[ 2] = f[ 2]; m[ 3] = f[ 3];
		m[ 4] = f[ 4]; m[ 5] = f[ 5]; m[ 6] = f[ 6]; m[ 7] = f[ 7];
		m[ 8] = f[ 8]; m[ 9] = f[ 9]; m[10] = f[10]; m[11] = f[11];
		m[12] = f[12]; m[13] = f[13]; m[14] = f[14]; m[15] = f[15];
	}
	void set(float  m0, float  m1, float  m2, float  m3,
	         float  m4, float  m5, float  m6, float  m7,
	         float  m8, float  m9, float m10, float m11,
	         float m12, float m13, float m14, float m15)
	{
		m[ 0] =  m0; m[ 1] =  m1; m[ 2] =  m2; m[ 3] =  m3;
		m[ 4] =  m4; m[ 5] =  m5; m[ 6] =  m6; m[ 7] =  m7;
		m[ 8] =  m8; m[ 9] =  m9; m[10] = m10; m[11] = m11;
		m[12] = m12; m[13] = m13; m[14] = m14; m[15] = m15;
	}
	void set(const mat4 &M)
	{
		m[ 0] = M.m[ 0]; m[ 1] = M.m[ 1]; m[ 2] = M.m[ 2]; m[ 3] = M.m[ 3];
		m[ 4] = M.m[ 4]; m[ 5] = M.m[ 5]; m[ 6] = M.m[ 6]; m[ 7] = M.m[ 7];
		m[ 8] = M.m[ 8]; m[ 9] = M.m[ 9]; m[10] = M.m[10]; m[11] = M.m[11];
		m[12] = M.m[12]; m[13] = M.m[13]; m[14] = M.m[14]; m[15] = M.m[15];
	}

	void set(const mat2 &M);
	void set(const mat3 &M);
	void set(const quat &q);

	mat4(float f)        { set(f); }
	mat4(const float *f) { set(f); }
	mat4(const mat4 &M)  { set(M); }
	mat4(const mat2 &M)  { set(M); }
	mat4(const mat3 &M)  { set(M); }
	mat4(const quat &q)  { set(q); }

	mat4(float  m0, float  m1, float  m2, float  m3,
	     float  m4, float  m5, float  m6, float  m7,
	     float  m8, float  m9, float m10, float m11,
	     float m12, float m13, float m14, float m15)
	{
		set( m0,  m1,  m2,  m3,
		     m4,  m5,  m6,  m7,
		     m8,  m9, m10, m11,
		    m12, m13, m14, m15);
	}

	mat4& operator=(float f)       { set(f); return *this; }
	mat4& operator=(const mat4 &M) { set(M); return *this; }
	mat4& operator=(const mat2 &M) { set(M); return *this; }
	mat4& operator=(const mat3 &M) { set(M); return *this; }
	mat4& operator=(const quat &q) { set(q); return *this; }

	float operator[](int i)       { return m[i]; }
	float operator[](int i) const { return m[i]; }

	const mat4 operator+(const mat4 &M) const
	{
		return mat4(m[ 0] + M.m[ 0], m[ 1] + M.m[ 1], m[ 2] + M.m[ 2], m[ 3] + M.m[ 3],
		            m[ 4] + M.m[ 4], m[ 5] + M.m[ 5], m[ 6] + M.m[ 6], m[ 7] + M.m[ 7],
		            m[ 8] + M.m[ 8], m[ 9] + M.m[ 9], m[10] + M.m[10], m[11] + M.m[11],
		            m[12] + M.m[12], m[13] + M.m[13], m[14] + M.m[14], m[15] + M.m[15]);
	}
	const mat4 operator-(const mat4 &M) const
	{
		return mat4(m[ 0] - M.m[ 0], m[ 1] - M.m[ 1], m[ 2] - M.m[ 2], m[ 3] - M.m[ 3],
		            m[ 4] - M.m[ 4], m[ 5] - M.m[ 5], m[ 6] - M.m[ 6], m[ 7] - M.m[ 7],
		            m[ 8] - M.m[ 8], m[ 9] - M.m[ 9], m[10] - M.m[10], m[11] - M.m[11],
		            m[12] - M.m[12], m[13] - M.m[13], m[14] - M.m[14], m[15] - M.m[15]);
	}
	const mat4 operator*(const mat4 &M) const
	{
		return mat4(m[ 0] * M.m[ 0] + m[ 1] * M.m[ 4] + m[ 2] * M.m[ 8] + m[ 3] * M.m[12],
		            m[ 0] * M.m[ 1] + m[ 1] * M.m[ 5] + m[ 2] * M.m[ 9] + m[ 3] * M.m[13],
		            m[ 0] * M.m[ 2] + m[ 1] * M.m[ 6] + m[ 2] * M.m[10] + m[ 3] * M.m[14],
		            m[ 0] * M.m[ 3] + m[ 1] * M.m[ 7] + m[ 2] * M.m[11] + m[ 3] * M.m[15],
		            m[ 4] * M.m[ 0] + m[ 5] * M.m[ 4] + m[ 6] * M.m[ 8] + m[ 7] * M.m[12],
		            m[ 4] * M.m[ 1] + m[ 5] * M.m[ 5] + m[ 6] * M.m[ 9] + m[ 7] * M.m[13],
		            m[ 4] * M.m[ 2] + m[ 5] * M.m[ 6] + m[ 6] * M.m[10] + m[ 7] * M.m[14],
		            m[ 4] * M.m[ 3] + m[ 5] * M.m[ 7] + m[ 6] * M.m[11] + m[ 7] * M.m[15],
		            m[ 8] * M.m[ 0] + m[ 9] * M.m[ 4] + m[10] * M.m[ 8] + m[11] * M.m[12],
		            m[ 8] * M.m[ 1] + m[ 9] * M.m[ 5] + m[10] * M.m[ 9] + m[11] * M.m[13],
		            m[ 8] * M.m[ 2] + m[ 9] * M.m[ 6] + m[10] * M.m[10] + m[11] * M.m[14],
		            m[ 8] * M.m[ 3] + m[ 9] * M.m[ 7] + m[10] * M.m[11] + m[11] * M.m[15],
		            m[12] * M.m[ 0] + m[13] * M.m[ 4] + m[14] * M.m[ 8] + m[15] * M.m[12],
		            m[12] * M.m[ 1] + m[13] * M.m[ 5] + m[14] * M.m[ 9] + m[15] * M.m[13],
		            m[12] * M.m[ 2] + m[13] * M.m[ 6] + m[14] * M.m[10] + m[15] * M.m[14],
		            m[12] * M.m[ 3] + m[13] * M.m[ 7] + m[14] * M.m[11] + m[15] * M.m[15]);
	}
	const mat4 operator*(float f) const
	{
		return mat4(m[ 0] * f, m[ 1] * f, m[ 2] * f, m[ 3] * f,
		            m[ 4] * f, m[ 5] * f, m[ 6] * f, m[ 7] * f,
		            m[ 8] * f, m[ 9] * f, m[10] * f, m[11] * f,
					m[12] * f, m[13] * f, m[14] * f, m[15] * f);
	}
	const mat4 operator/(float f) const
	{
		return mat4(m[ 0] / f, m[ 1] / f, m[ 2] / f, m[ 3] / f,
		            m[ 4] / f, m[ 5] / f, m[ 6] / f, m[ 7] / f,
		            m[ 8] / f, m[ 9] / f, m[10] / f, m[11] / f,
		            m[12] / f, m[13] / f, m[14] / f, m[15] / f);
	}

	mat4& operator+=(const mat4 &M)
	{
		m[ 0] += M.m[ 0]; m[ 1] += M.m[ 1]; m[ 2] += M.m[ 2]; m[ 3] += M.m[ 3];
		m[ 4] += M.m[ 4]; m[ 5] += M.m[ 5]; m[ 6] += M.m[ 6]; m[ 7] += M.m[ 7];
		m[ 8] += M.m[ 8]; m[ 9] += M.m[ 9]; m[10] += M.m[10]; m[11] += M.m[11];
		m[12] += M.m[12]; m[13] += M.m[13]; m[14] += M.m[14]; m[15] += M.m[15];
		return *this;
	}
	mat4& operator-=(const mat4 &M)
	{
		m[ 0] -= M.m[ 0]; m[ 1] -= M.m[ 1]; m[ 2] -= M.m[ 2]; m[ 3] -= M.m[ 3];
		m[ 4] -= M.m[ 4]; m[ 5] -= M.m[ 5]; m[ 6] -= M.m[ 6]; m[ 7] -= M.m[ 7];
		m[ 8] -= M.m[ 8]; m[ 9] -= M.m[ 9]; m[10] -= M.m[10]; m[11] -= M.m[11];
		m[12] -= M.m[12]; m[13] -= M.m[13]; m[14] -= M.m[14]; m[15] -= M.m[15];
		return *this;
	}
	mat4& operator*=(const mat4 &M)
	{
		set(m[ 0] * M.m[ 0] + m[ 1] * M.m[ 4] + m[ 2] * M.m[ 8] + m[ 3] * M.m[12],
		    m[ 0] * M.m[ 1] + m[ 1] * M.m[ 5] + m[ 2] * M.m[ 9] + m[ 3] * M.m[13],
		    m[ 0] * M.m[ 2] + m[ 1] * M.m[ 6] + m[ 2] * M.m[10] + m[ 3] * M.m[14],
		    m[ 0] * M.m[ 3] + m[ 1] * M.m[ 7] + m[ 2] * M.m[11] + m[ 3] * M.m[15],
		    m[ 4] * M.m[ 0] + m[ 5] * M.m[ 4] + m[ 6] * M.m[ 8] + m[ 7] * M.m[12],
		    m[ 4] * M.m[ 1] + m[ 5] * M.m[ 5] + m[ 6] * M.m[ 9] + m[ 7] * M.m[13],
		    m[ 4] * M.m[ 2] + m[ 5] * M.m[ 6] + m[ 6] * M.m[10] + m[ 7] * M.m[14],
		    m[ 4] * M.m[ 3] + m[ 5] * M.m[ 7] + m[ 6] * M.m[11] + m[ 7] * M.m[15],
		    m[ 8] * M.m[ 0] + m[ 9] * M.m[ 4] + m[10] * M.m[ 8] + m[11] * M.m[12],
		    m[ 8] * M.m[ 1] + m[ 9] * M.m[ 5] + m[10] * M.m[ 9] + m[11] * M.m[13],
		    m[ 8] * M.m[ 2] + m[ 9] * M.m[ 6] + m[10] * M.m[10] + m[11] * M.m[14],
		    m[ 8] * M.m[ 3] + m[ 9] * M.m[ 7] + m[10] * M.m[11] + m[11] * M.m[15],
		    m[12] * M.m[ 0] + m[13] * M.m[ 4] + m[14] * M.m[ 8] + m[15] * M.m[12],
		    m[12] * M.m[ 1] + m[13] * M.m[ 5] + m[14] * M.m[ 9] + m[15] * M.m[13],
		    m[12] * M.m[ 2] + m[13] * M.m[ 6] + m[14] * M.m[10] + m[15] * M.m[14],
		    m[12] * M.m[ 3] + m[13] * M.m[ 7] + m[14] * M.m[11] + m[15] * M.m[15]);
		return *this;
	}
	mat4& operator*=(float f)
	{
		m[ 0] *= f; m[ 1] *= f; m[ 2] *= f; m[ 3] *= f;
		m[ 4] *= f; m[ 5] *= f; m[ 6] *= f; m[ 7] *= f;
		m[ 8] *= f; m[ 9] *= f; m[10] *= f; m[11] *= f;
		m[12] *= f; m[13] *= f; m[14] *= f; m[15] *= f;
		return *this;
	}
	mat4& operator/=(float f)
	{
		m[ 0] /= f; m[ 1] /= f; m[ 2] /= f; m[ 3] /= f;
		m[ 4] /= f; m[ 5] /= f; m[ 6] /= f; m[ 7] /= f;
		m[ 8] /= f; m[ 9] /= f; m[10] /= f; m[11] /= f;
		m[12] /= f; m[13] /= f; m[14] /= f; m[15] /= f;
		return *this;
	}
};

inline float determinant(const mat4 &M)
{
	float A, B, C, D, E, F;

	A = M.m[10] * M.m[15] - M.m[11] * M.m[14];
	B = M.m[ 9] * M.m[15] - M.m[11] * M.m[13];
	C = M.m[ 9] * M.m[14] - M.m[10] * M.m[13];
	D = M.m[ 8] * M.m[15] - M.m[11] * M.m[12];
	E = M.m[ 8] * M.m[14] - M.m[10] * M.m[12];
	F = M.m[ 8] * M.m[13] - M.m[ 9] * M.m[12];

	return M.m[0] * (A * M.m[5] - B * M.m[6] + C * M.m[7])
		 - M.m[1] * (A * M.m[4] - D * M.m[6] + E * M.m[7])
		 + M.m[2] * (B * M.m[4] - D * M.m[5] + F * M.m[7])
		 - M.m[3] * (C * M.m[4] - E * M.m[5] + F * M.m[6]);
}

inline const mat4 inverse(const mat4 &M)
{
	const float det = 1.0f / determinant(M);

	#define MDET3(a0,a1,a2,a3,a4,a5,a6,a7,a8) \
				( a0 * (a4 * a8 - a5 * a7) \
				- a1 * (a3 * a8 - a5 * a6) \
				+ a2 * (a3 * a7 - a4 * a6) )

	// ~ 160 mul & 80 add
	return mat4(
		 MDET3(M.m[5], M.m[6], M.m[7], M.m[9], M.m[10], M.m[11], M.m[13], M.m[14], M.m[15]) * det,
		-MDET3(M.m[1], M.m[2], M.m[3], M.m[9], M.m[10], M.m[11], M.m[13], M.m[14], M.m[15]) * det,
		 MDET3(M.m[1], M.m[2], M.m[3], M.m[5], M.m[ 6], M.m[ 7], M.m[13], M.m[14], M.m[15]) * det,
		-MDET3(M.m[1], M.m[2], M.m[3], M.m[5], M.m[ 6], M.m[ 7], M.m[ 9], M.m[10], M.m[11]) * det,
		-MDET3(M.m[4], M.m[6], M.m[7], M.m[8], M.m[10], M.m[11], M.m[12], M.m[14], M.m[15]) * det,
		 MDET3(M.m[0], M.m[2], M.m[3], M.m[8], M.m[10], M.m[11], M.m[12], M.m[14], M.m[15]) * det,
		-MDET3(M.m[0], M.m[2], M.m[3], M.m[4], M.m[ 6], M.m[ 7], M.m[12], M.m[14], M.m[15]) * det,
		 MDET3(M.m[0], M.m[2], M.m[3], M.m[4], M.m[ 6], M.m[ 7], M.m[ 8], M.m[10], M.m[11]) * det,
		 MDET3(M.m[4], M.m[5], M.m[7], M.m[8], M.m[ 9], M.m[11], M.m[12], M.m[13], M.m[15]) * det,
		-MDET3(M.m[0], M.m[1], M.m[3], M.m[8], M.m[ 9], M.m[11], M.m[12], M.m[13], M.m[15]) * det,
		 MDET3(M.m[0], M.m[1], M.m[3], M.m[4], M.m[ 5], M.m[ 7], M.m[12], M.m[13], M.m[15]) * det,
		-MDET3(M.m[0], M.m[1], M.m[3], M.m[4], M.m[ 5], M.m[ 7], M.m[ 8], M.m[ 9], M.m[11]) * det,
		-MDET3(M.m[4], M.m[5], M.m[6], M.m[8], M.m[ 9], M.m[10], M.m[12], M.m[13], M.m[14]) * det,
		 MDET3(M.m[0], M.m[1], M.m[2], M.m[8], M.m[ 9], M.m[10], M.m[12], M.m[13], M.m[14]) * det,
		-MDET3(M.m[0], M.m[1], M.m[2], M.m[4], M.m[ 5], M.m[ 6], M.m[12], M.m[13], M.m[14]) * det,
		 MDET3(M.m[0], M.m[1], M.m[2], M.m[4], M.m[ 5], M.m[ 6], M.m[ 8], M.m[ 9], M.m[10]) * det);

	#undef MDET3
}

inline const mat4 transpose(const mat4 &M)
{
	return mat4(M.m[ 0], M.m[ 4], M.m[ 8], M.m[12],
				M.m[ 1], M.m[ 5], M.m[ 9], M.m[13],
				M.m[ 2], M.m[ 6], M.m[10], M.m[14],
				M.m[ 3], M.m[ 7], M.m[11], M.m[15]);
}

#endif /* MAT4_H */
