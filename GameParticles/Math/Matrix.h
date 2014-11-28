#ifndef MATRIX_H
#define MATRIX_H

// includes
#include "Enum.h"
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
	Matrix(const Vect4D& row1, const Vect4D& row2, const Vect4D& row3, const Vect4D& row4);
	~Matrix();

	Vect4D getRow(const MatrixRowEnum& row) const;

	void setTransMatrix(const Vect4D& t);
	void setScaleMatrix(const Vect4D& s);
	void setRotZMatrix(const float Z_Radians);

	float& operator[](INDEX_ENUM e);
	
	Matrix& operator*=(const Matrix& t);
	Matrix& operator-=(const Matrix& t);

	float Determinant() const;
	
	Matrix GetAdjugate() const;
	Matrix& Matrix::operator/=(const float t);
	
	void Matrix::Inverse(Matrix &out) const;
	
private:

#pragma warning(push)
#pragma warning(disable : 4201)
	union __declspec(align(16)) {
		__declspec(align(16)) __m128 row0;
		__declspec(align(16)) struct {
			float m0;
			float m1;
			float m2;
			float m3;
		};
	};

	union __declspec(align(16)) {
		__declspec(align(16)) __m128 row1;
		__declspec(align(16)) struct {
			float m4;
			float m5;
			float m6;
			float m7;
		};
	};

	union __declspec(align(16)) {
		__declspec(align(16)) __m128 row2;
		__declspec(align(16)) struct {
			float m8;
			float m9;
			float m10;
			float m11;
		};
	};

	union __declspec(align(16)) {
		__declspec(align(16)) __m128 row3;
		__declspec(align(16)) struct {
			float m12;
			float m13;
			float m14;
			float m15;
		};
	};
#pragma warning(pop)
};

#endif  // Matrix.h
