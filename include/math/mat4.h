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
	union
	{
		struct { float r1[4], r2[4], r3[4], r4[4]; };
		float m[16];
	};

	mat4() {}

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
		m[ 0] = M[ 0]; m[ 1] = M[ 1]; m[ 2] = M[ 2]; m[ 3] = M[ 3];
		m[ 4] = M[ 4]; m[ 5] = M[ 5]; m[ 6] = M[ 6]; m[ 7] = M[ 7];
		m[ 8] = M[ 8]; m[ 9] = M[ 9]; m[10] = M[10]; m[11] = M[11];
		m[12] = M[12]; m[13] = M[13]; m[14] = M[14]; m[15] = M[15];
	}

	void set(const mat2 &M);
	void set(const mat3 &M);
	void set(const quat &q);

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

	mat4& operator=(const mat4 &M) { set(M); return *this; }
	mat4& operator=(const mat2 &M) { set(M); return *this; }
	mat4& operator=(const mat3 &M) { set(M); return *this; }
	mat4& operator=(const quat &q) { set(q); return *this; }

	float operator[](int i)       { return m[i]; }
	float operator[](int i) const { return m[i]; }

	const mat4 operator+(const mat4 &M) const
	{
		return mat4(m[ 0] + M[ 0], m[ 1] + M[ 1], m[ 2] + M[ 2], m[ 3] + M[ 3],
		            m[ 4] + M[ 4], m[ 5] + M[ 5], m[ 6] + M[ 6], m[ 7] + M[ 7],
		            m[ 8] + M[ 8], m[ 9] + M[ 9], m[10] + M[10], m[11] + M[11],
		            m[12] + M[12], m[13] + M[13], m[14] + M[14], m[15] + M[15]);
	}
	const mat4 operator-(const mat4 &M) const
	{
		return mat4(m[ 0] - M[ 0], m[ 1] - M[ 1], m[ 2] - M[ 2], m[ 3] - M[ 3],
		            m[ 4] - M[ 4], m[ 5] - M[ 5], m[ 6] - M[ 6], m[ 7] - M[ 7],
		            m[ 8] - M[ 8], m[ 9] - M[ 9], m[10] - M[10], m[11] - M[11],
		            m[12] - M[12], m[13] - M[13], m[14] - M[14], m[15] - M[15]);
	}
	const mat4 operator*(const mat4 &M) const
	{
		return mat4(m[ 0] * M[ 0] + m[ 1] * M[ 4] + m[ 2] * M[ 8] + m[ 3] * M[12],
		            m[ 0] * M[ 1] + m[ 1] * M[ 5] + m[ 2] * M[ 9] + m[ 3] * M[13],
		            m[ 0] * M[ 2] + m[ 1] * M[ 6] + m[ 2] * M[10] + m[ 3] * M[14],
		            m[ 0] * M[ 3] + m[ 1] * M[ 7] + m[ 2] * M[11] + m[ 3] * M[15],
		            m[ 4] * M[ 0] + m[ 5] * M[ 4] + m[ 6] * M[ 8] + m[ 7] * M[12],
		            m[ 4] * M[ 1] + m[ 5] * M[ 5] + m[ 6] * M[ 9] + m[ 7] * M[13],
		            m[ 4] * M[ 2] + m[ 5] * M[ 6] + m[ 6] * M[10] + m[ 7] * M[14],
		            m[ 4] * M[ 3] + m[ 5] * M[ 7] + m[ 6] * M[11] + m[ 7] * M[15],
		            m[ 8] * M[ 0] + m[ 9] * M[ 4] + m[10] * M[ 8] + m[11] * M[12],
		            m[ 8] * M[ 1] + m[ 9] * M[ 5] + m[10] * M[ 9] + m[11] * M[13],
		            m[ 8] * M[ 2] + m[ 9] * M[ 6] + m[10] * M[10] + m[11] * M[14],
		            m[ 8] * M[ 3] + m[ 9] * M[ 7] + m[10] * M[11] + m[11] * M[15],
		            m[12] * M[ 0] + m[13] * M[ 4] + m[14] * M[ 8] + m[15] * M[12],
		            m[12] * M[ 1] + m[13] * M[ 5] + m[14] * M[ 9] + m[15] * M[13],
		            m[12] * M[ 2] + m[13] * M[ 6] + m[14] * M[10] + m[15] * M[14],
		            m[12] * M[ 3] + m[13] * M[ 7] + m[14] * M[11] + m[15] * M[15]);
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
		m[ 0] += M[ 0]; m[ 1] += M[ 1]; m[ 2] += M[ 2]; m[ 3] += M[ 3];
		m[ 4] += M[ 4]; m[ 5] += M[ 5]; m[ 6] += M[ 6]; m[ 7] += M[ 7];
		m[ 8] += M[ 8]; m[ 9] += M[ 9]; m[10] += M[10]; m[11] += M[11];
		m[12] += M[12]; m[13] += M[13]; m[14] += M[14]; m[15] += M[15];
		return *this;
	}
	mat4& operator-=(const mat4 &M)
	{
		m[ 0] -= M[ 0]; m[ 1] -= M[ 1]; m[ 2] -= M[ 2]; m[ 3] -= M[ 3];
		m[ 4] -= M[ 4]; m[ 5] -= M[ 5]; m[ 6] -= M[ 6]; m[ 7] -= M[ 7];
		m[ 8] -= M[ 8]; m[ 9] -= M[ 9]; m[10] -= M[10]; m[11] -= M[11];
		m[12] -= M[12]; m[13] -= M[13]; m[14] -= M[14]; m[15] -= M[15];
		return *this;
	}
	mat4& operator*=(const mat4 &M)
	{
		set(m[ 0] * M[ 0] + m[ 1] * M[ 4] + m[ 2] * M[ 8] + m[ 3] * M[12],
		    m[ 0] * M[ 1] + m[ 1] * M[ 5] + m[ 2] * M[ 9] + m[ 3] * M[13],
		    m[ 0] * M[ 2] + m[ 1] * M[ 6] + m[ 2] * M[10] + m[ 3] * M[14],
		    m[ 0] * M[ 3] + m[ 1] * M[ 7] + m[ 2] * M[11] + m[ 3] * M[15],
		    m[ 4] * M[ 0] + m[ 5] * M[ 4] + m[ 6] * M[ 8] + m[ 7] * M[12],
		    m[ 4] * M[ 1] + m[ 5] * M[ 5] + m[ 6] * M[ 9] + m[ 7] * M[13],
		    m[ 4] * M[ 2] + m[ 5] * M[ 6] + m[ 6] * M[10] + m[ 7] * M[14],
		    m[ 4] * M[ 3] + m[ 5] * M[ 7] + m[ 6] * M[11] + m[ 7] * M[15],
		    m[ 8] * M[ 0] + m[ 9] * M[ 4] + m[10] * M[ 8] + m[11] * M[12],
		    m[ 8] * M[ 1] + m[ 9] * M[ 5] + m[10] * M[ 9] + m[11] * M[13],
		    m[ 8] * M[ 2] + m[ 9] * M[ 6] + m[10] * M[10] + m[11] * M[14],
		    m[ 8] * M[ 3] + m[ 9] * M[ 7] + m[10] * M[11] + m[11] * M[15],
		    m[12] * M[ 0] + m[13] * M[ 4] + m[14] * M[ 8] + m[15] * M[12],
		    m[12] * M[ 1] + m[13] * M[ 5] + m[14] * M[ 9] + m[15] * M[13],
		    m[12] * M[ 2] + m[13] * M[ 6] + m[14] * M[10] + m[15] * M[14],
		    m[12] * M[ 3] + m[13] * M[ 7] + m[14] * M[11] + m[15] * M[15]);
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

	A = M[10] * M[15] - M[11] * M[14];
	B = M[ 9] * M[15] - M[11] * M[13];
	C = M[ 9] * M[14] - M[10] * M[13];
	D = M[ 8] * M[15] - M[11] * M[12];
	E = M[ 8] * M[14] - M[10] * M[12];
	F = M[ 8] * M[13] - M[ 9] * M[12];

	return M[0] * (A * M[5] - B * M[6] + C * M[7])
	     - M[1] * (A * M[4] - D * M[6] + E * M[7])
	     + M[2] * (B * M[4] - D * M[5] + F * M[7])
	     - M[3] * (C * M[4] - E * M[5] + F * M[6]);
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
		 MDET3(M[5], M[6], M[7], M[9], M[10], M[11], M[13], M[14], M[15]) * det,
		-MDET3(M[1], M[2], M[3], M[9], M[10], M[11], M[13], M[14], M[15]) * det,
		 MDET3(M[1], M[2], M[3], M[5], M[ 6], M[ 7], M[13], M[14], M[15]) * det,
		-MDET3(M[1], M[2], M[3], M[5], M[ 6], M[ 7], M[ 9], M[10], M[11]) * det,
		-MDET3(M[4], M[6], M[7], M[8], M[10], M[11], M[12], M[14], M[15]) * det,
		 MDET3(M[0], M[2], M[3], M[8], M[10], M[11], M[12], M[14], M[15]) * det,
		-MDET3(M[0], M[2], M[3], M[4], M[ 6], M[ 7], M[12], M[14], M[15]) * det,
		 MDET3(M[0], M[2], M[3], M[4], M[ 6], M[ 7], M[ 8], M[10], M[11]) * det,
		 MDET3(M[4], M[5], M[7], M[8], M[ 9], M[11], M[12], M[13], M[15]) * det,
		-MDET3(M[0], M[1], M[3], M[8], M[ 9], M[11], M[12], M[13], M[15]) * det,
		 MDET3(M[0], M[1], M[3], M[4], M[ 5], M[ 7], M[12], M[13], M[15]) * det,
		-MDET3(M[0], M[1], M[3], M[4], M[ 5], M[ 7], M[ 8], M[ 9], M[11]) * det,
		-MDET3(M[4], M[5], M[6], M[8], M[ 9], M[10], M[12], M[13], M[14]) * det,
		 MDET3(M[0], M[1], M[2], M[8], M[ 9], M[10], M[12], M[13], M[14]) * det,
		-MDET3(M[0], M[1], M[2], M[4], M[ 5], M[ 6], M[12], M[13], M[14]) * det,
		 MDET3(M[0], M[1], M[2], M[4], M[ 5], M[ 6], M[ 8], M[ 9], M[10]) * det);

	#undef MDET3
}

inline const mat4 transpose(const mat4 &M)
{
	return mat4(M[ 0], M[ 4], M[ 8], M[12],
	            M[ 1], M[ 5], M[ 9], M[13],
	            M[ 2], M[ 6], M[10], M[14],
	            M[ 3], M[ 7], M[11], M[15]);
}

#endif /* MAT4_H */
