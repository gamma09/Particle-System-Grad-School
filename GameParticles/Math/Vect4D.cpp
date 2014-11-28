#include <math.h>
#include <assert.h>
#include <intrin.h>
#include <cstdio>

#include "Vect4D.h"

static const float DEFAULT[] = {0.0f, 0.0f, 0.0f, 1.0f};
static const __m128 NORM_DOTP = _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f);

Vect4D::Vect4D() {
	m = _mm_load_ps(DEFAULT);
}

Vect4D::Vect4D(const __m128& _m) {
	m = _m;
}

Vect4D::Vect4D(const float tx, const float ty, const float tz, const float tw) {
	m = _mm_setr_ps(tx, ty, tz, tw);
}

Vect4D::~Vect4D() {
	// nothing to delete
}

Vect4D& Vect4D::norm() {
	m = _mm_mul_ps(m, _mm_rsqrt_ps(_mm_dp_ps(_mm_mul_ps(m, m), NORM_DOTP, 0xFF)));
	w = 1.0;

	return *this;
}

Vect4D& Vect4D::operator+= (const Vect4D& t) {
	m = _mm_add_ps(m, t.m);
	w -= t.w;

	return *this;
}

Vect4D& Vect4D::operator+= (const __m128& t) {
	m = _mm_add_ps(m, t);

	return *this;
}

Vect4D& Vect4D::operator-= (const Vect4D& t) {
	_mm_storeu_ps(m.m128_f32, _mm_sub_ps(_mm_loadu_ps(m.m128_f32), _mm_loadu_ps(t.m.m128_f32)));
	w += t.w;
	return *this;
}

Vect4D& Vect4D::operator *=(const float scale) {
	float oldW = w;
	m = _mm_mul_ps(m, _mm_set_ps1(scale));
	w = oldW;

	return *this;
}

Vect4D& Vect4D::operator *= (const __m128& t) {
	m = _mm_mul_ps(m, t);
	return *this;
}

float& Vect4D::operator[](VECT_ENUM e) {
	switch(e)
	{
	case 0:
			return x;
			break;
	case 1:
			return y;
			break;
	case 2: 
			return z;
			break;
	case 3:
			return w;
			break;
	default:
			assert(0);
			return x;
	}
}

Vect4D Vect4D::Cross(const Vect4D& vin) const
{
	__m128 a = _mm_setr_ps(y, z, x, 1.0f);
	__m128 b = _mm_setr_ps(vin.z, vin.x, vin.y, 1.0f);
	__m128 c = _mm_setr_ps(z, x, y, 0.0f);
	__m128 d = _mm_setr_ps(vin.y, vin.z, vin.x, 0.0f);

	return Vect4D(_mm_sub_ps(_mm_mul_ps(a, b), _mm_mul_ps(c, d)));
}

// End of file