// Vect4D.h
//
// This is a 4 dimensional Vect4D class
#ifndef Vect4D_H
#define Vect4D_H

// includes
#include "Enum.h"
#include <xmmintrin.h>

// Foward Declarations
class Matrix;

// class
class __declspec(align(16)) Vect4D
{
public:
	friend class Matrix;

	Vect4D();	
	Vect4D(const float tx, const float ty, const float tz, const float tw = 1.0f);
	Vect4D(const __m128& m);
	~Vect4D();

	Vect4D& norm();
	
	Vect4D& operator += (const Vect4D& t);
	Vect4D& operator += (const __m128& t);
	Vect4D& operator -= (const Vect4D& t);
	Vect4D& operator *= (const float scale);
	Vect4D& operator *= (const __m128& t);

	Vect4D Cross(const Vect4D &vin) const;

	float &operator[](VECT_ENUM e);

private:
#pragma warning(push)
#pragma warning(disable : 4201)
	union __declspec(align(16)) {
		__declspec(align(16)) __m128 m;

		__declspec(align(16)) struct {
			float x;
			float y;
			float z;
			float w;
		};
	};
#pragma warning(pop)
};

#endif  //Vect4D.h
