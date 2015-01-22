#include <Math.h>
#include <assert.h>
#include "Vect4d.h"
#include "Matrix.h"
#include <intrin.h>

static const __m128 IDENTITY_ROW_0 = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
static const __m128 IDENTITY_ROW_1 = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
static const __m128 IDENTITY_ROW_2 = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
static const __m128 IDENTITY_ROW_3 = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

// Default constructor
Matrix::Matrix() :
	row0(IDENTITY_ROW_0),
	row1(IDENTITY_ROW_1),
	row2(IDENTITY_ROW_2),
	row3(IDENTITY_ROW_3)
{
}


// copy constructor
Matrix::Matrix(const Matrix& t) :
	row0(t.row0),
	row1(t.row1),
	row2(t.row2),
	row3(t.row3)
{
}

// Specialize constructor
// Initializes the matrix have the specified rows
Matrix::Matrix(const Vect4D& _row0, const Vect4D& _row1, const Vect4D& _row2, const Vect4D& _row3) 
	: row0(_row0.m), row1(_row1.m), row2(_row2.m), row3(_row3.m) {
}

// Creates a scale matrix, which has the following form:
// [ sx  0  0  0 ]
// [  0 sy  0  0 ]
// [  0  0 sz  0 ]
// [  0  0  0  1 ]
Matrix::Matrix(const Vect4D& scale) :
	row0(_mm_set_ss(scale.x)),
	row1(_mm_setr_ps(0.0f, scale.y, 0.0f, 0.0f)),
	row2(_mm_setr_ps(0.0f, 0.0f, scale.z, 0.0f)),
	row3(IDENTITY_ROW_3)
{
}

// Destructor
Matrix::~Matrix()
{
	// nothing to delete
}

// This is a shortcut method for multiplying this matrix by a rotation
// matrix. It runs faster than the full matrix multiply method, as this
// method does not perform a full matrix multiplication. This method
// will only multiply two matrices and return the correct product when
// the matrices you would be multiplying have this form:
//
// [ m0 m4  m8 m12 ]     [ cos -sin  0  0 ]
// [ m1 m5  m9 m13 ]     [ sin  cos  0  0 ]
// [ m2 m6 m10 m14 ]  *  [  0    0   1  0 ]
// [ m3 m7 m11 m15 ]     [  0    0   0  1 ]
void Matrix::rotate(const float z_radians)
{
	float sn = sin(z_radians);
	__m128 cosine = _mm_set_ps1(cos(z_radians));
	__m128 sine = _mm_set_ps1(sn);

	sn *= -1;
	__m128 negSine = _mm_set_ps1(sn);

	__m128 col0 = _mm_setr_ps(m[0], m[4], m[8], m[12]);
	__m128 col1 = _mm_setr_ps(m[1], m[5], m[9], m[13]);

	__m128 result0 = _mm_add_ps(_mm_mul_ps(col0, cosine), _mm_mul_ps(col1, sine));
	__m128 result1 = _mm_add_ps(_mm_mul_ps(col0, negSine), _mm_mul_ps(col1, cosine));

	m[0] = result0.m128_f32[0];
	m[4] = result0.m128_f32[1];
	m[8] = result0.m128_f32[2];
	m[12] = result0.m128_f32[3];

	m[1] = result1.m128_f32[0];
	m[5] = result1.m128_f32[1];
	m[9] = result1.m128_f32[2];
	m[13] = result1.m128_f32[3];
}

// This is a shortcut for multiplying this matrix by a translation matrix.
// It runs faster than the full matrix multiply method, as this method does
// not perform a full matrix multiplication. This method will only multiply
// two matrices and return the correct product when the matrices you are
// multiplying have this form:
//
// [ m0 m4  m8 m12 ]     [ 1 0 0 x ]
// [ m1 m5  m9 m13 ]     [ 0 1 0 y ]
// [ m2 m6 m10 m14 ]  *  [ 0 0 1 z ]
// [ m3 m7 m11 m15 ]     [ 0 0 0 1 ]
void Matrix::translate(const Vect4D& t) {
	assert(t.w == 1.0f);

	__m128 col0 = _mm_setr_ps(m[0], m[4], m[8], m[12]);
	__m128 col1 = _mm_setr_ps(m[1], m[5], m[9], m[13]);
	__m128 col2 = _mm_setr_ps(m[2], m[6], m[10], m[14]);
	__m128 col3 = _mm_setr_ps(m[3], m[7], m[11], m[15]);

	m[12] = _mm_dp_ps(col0, t.m, 0xF1).m128_f32[0];
	m[13] = _mm_dp_ps(col1, t.m, 0xF1).m128_f32[0];
	m[14] = _mm_dp_ps(col2, t.m, 0xF1).m128_f32[0];
	m[15] = _mm_dp_ps(col3, t.m, 0xF1).m128_f32[0];
}

// Multiply two matrices together fully
// If possible, try to use shortcuts instead of this method, as they run faster
// due to assumptions about the input (see the translate() and rotate() methods).
Matrix& Matrix::operator*=(const Matrix& rhs) {
	// matrix multiplications
	// OPERATIONS: 24

	__m128 col0 = _mm_setr_ps(rhs.m[0], rhs.m[4], rhs.m[8], rhs.m[12]);
	__m128 col1 = _mm_setr_ps(rhs.m[1], rhs.m[5], rhs.m[9], rhs.m[13]);
	__m128 col2 = _mm_setr_ps(rhs.m[2], rhs.m[6], rhs.m[10], rhs.m[14]);
	__m128 col3 = _mm_setr_ps(rhs.m[3], rhs.m[7], rhs.m[11], rhs.m[15]);

	__m128 row0 = this->row0;
	__m128 row1 = this->row1;
	__m128 row2 = this->row2;
	__m128 row3 = this->row3;

	m[0]  = _mm_dp_ps(row0, col0, 0xF1).m128_f32[0];
	m[1]  = _mm_dp_ps(row0, col1, 0xF1).m128_f32[0];
	m[2]  = _mm_dp_ps(row0, col2, 0xF1).m128_f32[0];
	m[3]  = _mm_dp_ps(row0, col3, 0xF1).m128_f32[0];

	m[4]  = _mm_dp_ps(row1, col0, 0xF1).m128_f32[0];
	m[5]  = _mm_dp_ps(row1, col1, 0xF1).m128_f32[0];
	m[6]  = _mm_dp_ps(row1, col2, 0xF1).m128_f32[0];
	m[7]  = _mm_dp_ps(row1, col3, 0xF1).m128_f32[0];

	m[8]  = _mm_dp_ps(row2, col0, 0xF1).m128_f32[0];
	m[9]  = _mm_dp_ps(row2, col1, 0xF1).m128_f32[0];
	m[10] = _mm_dp_ps(row2, col2, 0xF1).m128_f32[0];
	m[11] = _mm_dp_ps(row2, col3, 0xF1).m128_f32[0];

	m[12] = _mm_dp_ps(row3, col0, 0xF1).m128_f32[0];
	m[13] = _mm_dp_ps(row3, col1, 0xF1).m128_f32[0];
	m[14] = _mm_dp_ps(row3, col2, 0xF1).m128_f32[0];
	m[15] = _mm_dp_ps(row3, col3, 0xF1).m128_f32[0];

	return *this;
}

// End of file