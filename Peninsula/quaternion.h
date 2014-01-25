#pragma once

#include <math.h>
#include <xmmintrin.h>
#include <pmmintrin.h>

__declspec (align(16))
struct Vector
{
	union {
		struct {
			float x, y, z, w;
		};

		struct {
			float i, j, k;
		};

		__m128 sse;
	};
};

__declspec (align(16))
struct Quaternion
{
	union {
		struct {
			float i, j, k, S;
		};

		__m128 sse;
	};
};

float VectorDot(const Vector& a, const Vector& b)
{
	__m128 value = _mm_mul_ps(a.sse, b.sse);
	value.m128_f32[3] = 0;
	value = _mm_hadd_ps(value, value);
	value = _mm_hadd_ps(value, value);
	return value.m128_f32[0];
}

Vector VectorCrossRH(const Vector& a, const Vector& b)
{
	Vector ret;

	ret.x = a.y * b.z - a.z * b.y;
	ret.y = a.x * b.z - a.z * b.x;
	ret.z = a.x * b.y - a.y * b.x;
	ret.w = 0.f;

	return ret;
}

Vector VectorCrossLH(const Vector& a, const Vector& b)
{
	Vector ret = VectorCrossRH(a, b);
	ret.y *= -1;
	return ret;
}

float VectorTripleScalarProductRH(const Vector& a, const Vector& b, const Vector& c)
{
	return VectorDot(a, VectorCrossRH(b, c));
}

float VectorTripleScalarProductLH(const Vector& a, const Vector& b, const Vector& c)
{
	return VectorDot(a, VectorCrossLH(b, c));
}

Quaternion QuaternionFromAxisAndAngle(const Vector& axis, float angle)
{
	float s = sin(angle / 2);
	float c = cos(angle / 2);

	Quaternion Q;
	Q.sse = axis.sse;
	
	__m128 multiply_mask = _mm_set_ps(0.f, s, s, s);

	Q.sse = _mm_mul_ps(Q.sse, multiply_mask);
	Q.S = c;

	return Q;
}

float QuaternionNorm(const Quaternion& q)
{
	__m128 result = _mm_mul_ps(q.sse, q.sse);
	
	result = _mm_hadd_ps(result, result);
	result = _mm_hadd_ps(result, result);

	return result.m128_f32[0];
}

float QuaternionLength(const Quaternion& q)
{
	return sqrtf(QuaternionNorm(q));
}

Quaternion QuaternionConjugate(const Quaternion& q)
{
	__m128 mask = _mm_set_ps(-1.f, -1.f, -1.f, 1.f);

	Quaternion ret;
	ret.sse = _mm_mul_ps(q.sse, mask);
	
	return q;
}

Quaternion QuaternionInverse(const Quaternion& q)
{
	Quaternion ret(QuaternionConjugate(q));

	float length = QuaternionNorm(q);

	__m128 mask = _mm_set_ps1(length);

	ret.sse = _mm_div_ps(ret.sse, mask);

	return ret;
}

Quaternion QuaternionNormalise(const Quaternion& q)
{
	Quaternion ret(q);

	float length = QuaternionNorm(q);

	__m128 mask = _mm_set_ps1(length);

	ret.sse = _mm_div_ps(ret.sse, mask);

	return ret;
}