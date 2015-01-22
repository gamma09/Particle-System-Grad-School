#ifndef MATRIX_H
#define MATRIX_H

// includes
#include <xmmintrin.h>
#include <cstdio>

// forward declare
class Vect4D;

// class
class __declspec(align(16)) Matrix
{
public:

	// local enumerations
	enum MatrixRowEnum
	{
		MATRIX_ROW_0,
		MATRIX_ROW_1,
		MATRIX_ROW_2,
		MATRIX_ROW_3
	};

	Matrix();	
	Matrix(const Matrix& t);
	Matrix(const Vect4D& scale);
	Matrix(const Vect4D& row1, const Vect4D& row2, const Vect4D& row3, const Vect4D& row4);
	~Matrix();

	void translate(const Vect4D& t);
	void rotate(const float z_radians);
	
	Matrix& operator*=(const Matrix& t);
	
private:

// Disable the warning for anonymous union/struct - these are supposed to be that way
#pragma warning(push)
#pragma warning(disable : 4201)
	union {
		struct {
			__m128 row0;
			__m128 row1;
			__m128 row2;
			__m128 row3;
		};

		float m[16];
	};
#pragma warning(pop)
};

#endif  // Matrix.h
