#include <Math.h>
#include <assert.h>
#include "Vect4d.h"
#include "Matrix.h"
#include <intrin.h>

static const __m128 IDENTITY_ROW_0 = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
static const __m128 IDENTITY_ROW_1 = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
static const __m128 IDENTITY_ROW_2 = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
static const __m128 IDENTITY_ROW_3 = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

static const __m128 DETERMINANT_DOT_P = _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f);

// Default constructor
Matrix::Matrix() : row0(IDENTITY_ROW_0), row1(IDENTITY_ROW_1), row2(IDENTITY_ROW_2), row3(IDENTITY_ROW_3) {
}


// copy constructor
Matrix::Matrix(const Matrix& t) : row0(t.row0), row1(t.row1), row2(t.row2), row3(t.row3) {
}

// Specialize constructor
Matrix::Matrix(const Vect4D& _row0, const Vect4D& _row1, const Vect4D& _row2, const Vect4D& _row3) 
	: row0(_row0.m), row1(_row1.m), row2(_row2.m), row3(_row3.m) {
}


Matrix::~Matrix() {
	// nothign to delete
}

void Matrix::setTransMatrix(const Vect4D& t) {
	// set the translation matrix (note: we are row major)

#ifdef _DEBUG
	assert(t.w == 1.0f);
#endif

	row3 = t.m;
}

float& Matrix::operator[](const INDEX_ENUM& e) {
	// get the individual elements
	switch(e)
	{
	case 0:
		return m0;
		break;
	case 1:
		return m1;
		break;
	case 2:
		return m2;
		break;
	case 3:
		return m3;
		break;
	case 4:
		return m4;
		break;
	case 5:
		return m5;
		break;
	case 6:
		return m6;
		break;
	case 7:
		return m7;
		break;
	case 8:
		return m8;
		break;
	case 9:
		return m9;
		break;	
	case 10:
		return m10;
		break;
	case 11:
		return m11;
		break;	
	case 12:
		return m12;
		break;	
	case 13:
		return m13;
		break;	
	case 14:
		return m14;
		break;
	case 15:
		return m15;
		break;
	default:
		assert(0);
		return m0;
		break;
	}
}


Vect4D Matrix::getRow(const MatrixRowEnum& row) const {
	switch( row )
	{
	case MATRIX_ROW_0:
		return Vect4D(row0);

	case MATRIX_ROW_1:
		return Vect4D(row1);

	case MATRIX_ROW_2:
		return Vect4D(row2);

	case MATRIX_ROW_3:
		return Vect4D(row3);

	default:
		assert(0);
		return Vect4D();
	}
}


Matrix& Matrix::operator*=(const Matrix& rhs) {
	// matrix multiplications
	// OPERATIONS: 24

	__m128 col0 = _mm_setr_ps(rhs.m0, rhs.m4, rhs.m8, rhs.m12);
	__m128 col1 = _mm_setr_ps(rhs.m1, rhs.m5, rhs.m9, rhs.m13);
	__m128 col2 = _mm_setr_ps(rhs.m2, rhs.m6, rhs.m10, rhs.m14);
	__m128 col3 = _mm_setr_ps(rhs.m3, rhs.m7, rhs.m11, rhs.m15);

	__m128 row0 = this->row0;
	__m128 row1 = this->row1;
	__m128 row2 = this->row2;
	__m128 row3 = this->row3;

	m0  = _mm_dp_ps(row0, col0, 0xF1).m128_f32[0];
	m1  = _mm_dp_ps(row0, col1, 0xF1).m128_f32[0];
	m2  = _mm_dp_ps(row0, col2, 0xF1).m128_f32[0];
	m3  = _mm_dp_ps(row0, col3, 0xF1).m128_f32[0];

	m4  = _mm_dp_ps(row1, col0, 0xF1).m128_f32[0];
	m5  = _mm_dp_ps(row1, col1, 0xF1).m128_f32[0];
	m6  = _mm_dp_ps(row1, col2, 0xF1).m128_f32[0];
	m7  = _mm_dp_ps(row1, col3, 0xF1).m128_f32[0];

	m8  = _mm_dp_ps(row2, col0, 0xF1).m128_f32[0];
	m9  = _mm_dp_ps(row2, col1, 0xF1).m128_f32[0];
	m10 = _mm_dp_ps(row2, col2, 0xF1).m128_f32[0];
	m11 = _mm_dp_ps(row2, col3, 0xF1).m128_f32[0];

	m12 = _mm_dp_ps(row3, col0, 0xF1).m128_f32[0];
	m13 = _mm_dp_ps(row3, col1, 0xF1).m128_f32[0];
	m14 = _mm_dp_ps(row3, col2, 0xF1).m128_f32[0];
	m15 = _mm_dp_ps(row3, col3, 0xF1).m128_f32[0];

	return *this;
}

Matrix& Matrix::operator-=(const Matrix& t) {
	// OPERATIONS: 4
	row0 = _mm_sub_ps(row0, t.row0);
	row1 = _mm_sub_ps(row1, t.row1);
	row2 = _mm_sub_ps(row2, t.row2);
	row3 = _mm_sub_ps(row3, t.row3);

	return *this;
}

Matrix& Matrix::operator/=(const float rhs) {
	// OPERATIONS: 6

	// divide each element by a value
	// using inverse multiply trick, faster that individual divides
#ifdef _DEBUG
	assert(fabs(rhs) > 0.0001f);
#endif

	__m128 mul = _mm_set_ps1(1.0f / rhs);
	row0 = _mm_mul_ps(row0, mul);
	row1 = _mm_mul_ps(row1, mul);
	row2 = _mm_mul_ps(row2, mul);
	row3 = _mm_mul_ps(row3, mul);

	return *this;
}

float Matrix::Determinant() const {
	// OPERATIONS: 27

	// A = { a,b,c,d / e,f,g,h / j,k,l,m / n,o,p,q }
	// A = { 0,1,2,3 / 4,5,6,7 / 8,9,10,11 / 12,13,14,15 }
	
	// det(A) = a*det( { f,g,h / k,l,m / o,p,q } )
	//			- b*det( { e,g,h / j,l,m / n,p,q } )
	//			+ c*det( { e,f,h / j,k,m / n,o,q } )
	//			- d*det( { e,f,g / j,k,l / n,o,p } )
	
	// det(A) = (a (f (lq - mp) - g (kq - mo) + h (kp - lo) ) )
	//			- (b (e (lq - mp) - g (jq - mn) + h (jp - ln) ) )
	//			+ (c (e (kq - mo) - f (jq - mn) + h (jo - kn) ) )
	//			- (d (e (kp - lo) - f (jp - ln) + g (jo - kn) ) )

	// Stage 1 calculations (calculate all of the 2x2 determinants that we'll need)
	__m128 t1a = _mm_mul_ps(_mm_set_ps(m10, m9, m9, m8), _mm_set_ps(m15, m15, m14, m15));
	__m128 t1b = _mm_mul_ps(_mm_set_ps1(m8), _mm_set_ps(m13, m14, 0.0f, 0.0f));
	__m128 t2a = _mm_mul_ps(_mm_set_ps(m11, m11, m10, m11), _mm_set_ps(m14, m13, m13, m12));
	__m128 t2b = _mm_mul_ps(_mm_set_ps(m9, m10, 0.0f, 0.0f), _mm_set_ps1(m12));
	// 12 operations

	__m128 va = _mm_sub_ps(t1a, t2a);
	__m128 vb = _mm_sub_ps(t1b, t2b);
	// 2 operations

	// Stage 2 calculations (calculate the overall determinant)
	__m128 v1 = _mm_set_ps(m4, m4, m4, m5);
	__m128 v2 = _mm_set_ps(va.m128_f32[1], va.m128_f32[2], va.m128_f32[3], va.m128_f32[3]);
	__m128 v3 = _mm_set_ps(m5, m5, m6, m6);
	__m128 v4 = _mm_set_ps(vb.m128_f32[2], va.m128_f32[0], va.m128_f32[0], va.m128_f32[2]);
	__m128 v5 = _mm_set_ps(m6, m7, m7, m7);
	__m128 v6 = _mm_set_ps(vb.m128_f32[3], vb.m128_f32[3], vb.m128_f32[2], va.m128_f32[1]);
	// 6 operations

	__m128 z = _mm_mul_ps(row0, _mm_add_ps(_mm_sub_ps(_mm_mul_ps(v1, v2), _mm_mul_ps(v3, v4)), _mm_mul_ps(v5, v6)));
	// 6 operations

	return _mm_dp_ps(z, DETERMINANT_DOT_P, 0xF1).m128_f32[0];
	// 1 operation

	// det(A) = (a (f*ta - g*tb + h*tc) )
	//		  - (b (e*ta - g*td + h*tf) )
	//		  + (c (e*tb - f*td + h*te) )
	//		  - (d (e*tc - f*tf + g*te) )
}

Matrix Matrix::GetAdjugate() const {
	// TODO: Can we optimize this any?


	// matrix = { a,b,c,d / e,f,g,h / j,k,l,m / n,o,p,q }
	
	// ta = lq - mp
	// tb = kq - mo
	// tc = kp - lo
	// td = gq - hp
	// te = fq - ho
	// tf = fp - go
	// tg = gm - hl
	// th = fm - hk
	// ti = fl - gk
	// tj = jq - mn
	// tk = jp - ln
	// tl = eq - hn
	// tm = ep - gn
	// tn = em - hj
	// to = el - gj
	// tp = jo - kn
	// tq = ek - fj
	// tr = eo - fn
	
	// a = det( { f,g,h / k,l,m / o,p,q } )
	// a = f(lq - mp) - g(kq - mo) + h(kp - lo)
	// a = f(ta) - g(tb) + h(tc)
	
	// b = -det( { b,c,d / k,l,m / o,p,q } )
	// b = -( b(lq - mp) - c(kq - mo) + d(kp - lo))
	// b = -( b(ta) - c(tb) + d(tc))
	
	// c = det( { b,c,d / f,g,h / o,p,q } )
	// c = b(gq - hp) - c(fq - ho) + d(fp - go)
	// c = b(td) - c(te) + d(tf)
	
	// d = -det( { b,c,d / f,g,h / k,l,m } )
	// d = -( b(gm - hl) - c(fm - hk) + d(fl - gk) )
	// d = -( b(tg) - c(th) + d(ti) )

	// e = -det( { e,g,h / j,l,m / n,p,q } )
	// e = - ( e(lq - mp) - g(jq - mn) + h(jp - ln))
	// e = - ( e(ta) - g(tj) + h(tk))
	
	// f = det( { a,c,d / j,l,m / n,p,q } )
	// f = a(lq - mp) - c(jq - mn) + d(jp - ln)
	// f = a(ta) - c(tj) + d(tk)
	
	// g = -det( { a,c,d / e,g,h / n,p,q } )
	// g = - ( a(gq - hp) - c(eq - hn) + d(ep - gn))
	// g = - ( a(td) - c(tl) + d(tm))
	
	// h = det( { a,c,d / e,g,h / j,l,m } )
	// h = a(gm - hl) - c(em - hj) + d(el - gj)
	// h = a(tg) - c(tn) + d(to)
	
	// j = det( { e,f,h / j,k,m / n,o,q } )
	// j = e(kq - mo) - f(jq - mn) + h(jo - kn)
	// j = e(tb) - f(tj) + h(tp)
	
	// k = -det( { a,b,d / j,k,m / n,o,q } )
	// k = - ( a(kq - mo) - b(jq - mn) + d(jo - kn))
	// k = - ( a(tb) - b(tj) + d(tp))
	
	// l = det( { a,b,d / e,f,h / n,o,q } )
	// l = a(fq - ho) - b(eq - hn) + d(eo - fn)
	// l = a(te) - b(tl) + d(tr)
	
	// m = -det( { a,b,d / e,f,h / j,k,m } )
	// m = -( a(fm - hk) - b(em - hj) + d(ek - fj))
	// m = -( a(th) - b(tn) + d(tq))
	
	// n = -det( { e,f,g / j,k,l / n,o,p } )
	// n = -( e(kp - lo) - f(jp - ln) + g(jo - kn))
	// n = -( e(tc) - f(tk) + g(tp))
	
	// o = det( { a,b,c / j,k,l / n,o,p } )
	// o = a(kp - lo) - b(jp - ln) + c(jo - kn)
	// o = a(tc) - b(tk) + c(tp)
	
	// p = -det( { a,b,c / e,f,g / n,o,p } )
	// p = - ( a(fp - go) - b(ep - gn) + c(eo - fn))
	// p = - ( a(tf) - b(tm) + c(tr))
	
	// q = det( { a,b,c / e,f,g / j,k,l } )
	// q = a(fl - gk) - b(el - gj) + c(ek - fj)
	// q = a(ti) - b(to) + c(tq)
	
	Matrix tmp;
	
	// load		ABC		(3)		ABC--
	float t1 = (m10*m15) - (m11*m14);
	float t2 = (m9*m15) - (m11*m13);
	float t3 = (m9*m14) - (m10*m13);
	
	// a = f(ta) - g(tb) + h(tc)
	tmp.m0 = (m5*t1) - (m6*t2) + (m7*t3);
	// b = -( b(ta) - c(tb) + d(tc))
	tmp.m1 = -((m1*t1) - (m2*t2) + (m3*t3));
	
	// load		JK		(5)		ABCJK
	float t4 = (m8*m15) - (m11*m12);
	float t5 = (m8*m14) - (m10*m12);
	// e = - ( e(ta) - g(tj) + h(tk))
	tmp.m4 = -( (m4*t1) - (m6*t4) + (m7*t5));
	// f = a(ta) - c(tj) + d(tk)
	tmp.m5 = (m0*t1) - (m2*t4) + (m3*t5);
	
	// unload	AJ		(3)		-BC-K
	// load		P		(4)		PBC-K
	t1 = (m8*m13) - (m9*m12);
	// n = -( e(tc) - f(tk) + g(tp))
	tmp.m12 = -((m4*t3) - (m5*t5) + (m6*t1));
	// o = a(tc) - b(tk) + c(tp)
	tmp.m13 = (m0*t3) - (m1*t5) + (m2*t1);
	
	// unload	KC		(2)		PB---
	// load		J		(3)		PBJ--
	t3 = (m8*m15) - (m11*m12);
	
	// j = e(tb) - f(tj) + h(tp)
	tmp.m8 = (m4*t2) - (m5*t3) + (m7*t1);
	// k = - ( a(tb) - b(tj) + d(tp))
	tmp.m9 = -((m0*t2) - (m1*t3) + (m3*t1));
	
	// unload	BPJ		(0)		-----
	// load		DLM		(3)		DLM--
	t1 = (m6*m15) - (m7*m14);
	t2 = (m4*m15) - (m7*m12);
	t3 = (m4*m14) - (m6*m12);
	
	// g = - ( a(td) - c(tl) + d(tm))
	tmp.m6 = -((m0*t1) - (m2*t2) + (m3*t3));
	
	// load		FR		(5)		DLMFR
	t4 = (m5*m14) - (m6*m13);
	t5 = (m4*m13) - (m5*m12);
	
	// p = - ( a(tf) - b(tm) + c(tr))
	tmp.m14 = -( (m0*t4) - (m1*t3) + (m3*t5));
	
	// unload	M		(4)		DL-FR
	// load		E		(5)		DLEFR
	t3 = (m5*m15) - (m7*m13);
	
	// l = a(te) - b(tl) + d(tr)
	tmp.m10 = (m0*t3) - (m1*t2) + (m3*t5);
	
	// unload	LR		(3)		D-EF-
	// c = b(td) - c(te) + d(tf)
	tmp.m2 = (m1*t1) - (m2*t3) + (m3*t4);
	
	// unload	DEF		(0)		-----
	// load		GHI		(3)		GHI--
	t1 = (m6*m11) - (m7*m10);
	t2 = (m5*m11) - (m7*m9);
	t3 = (m5*m10) - (m6*m9);
	
	// d = -( b(tg) - c(th) + d(ti) )
	tmp.m3 = -( (m1*t1) - (m2*t2) + (m3*t3));
	
	// load		NO		(5)		GHINO
	t4 = (m4*m11) - (m7*m8);
	t5 = (m4*m10) - (m6*m8);
	
	// h = a(tg) - c(tn) + d(to)
	tmp.m7 = (m0*t1) - (m2*t4) + (m3*t5);
	
	// unload	G		(4)		-HINO
	// load		Q		(5)		QHINO
	t1 = (m4*m9) - (m5*m8);
	
	// m = -( a(th) - b(tn) + d(tq))
	tmp.m11 = -((m0*t2) - (m1*t4) + (m3*t1));
	
	// unload	HN		(3)		Q-I-O
	// q = a(ti) - b(to) + c(tq)
	tmp.m15 = (m0*t3) - (m1*t5) + (m2*t1);
	
	return tmp;
}

void Matrix::Inverse(Matrix &out) const {
	float det = Determinant();

#ifdef _DEBUG
	assert(fabs(det) > 0.0001f);
#endif

	out = GetAdjugate();
	out /= det;
}

void Matrix::setScaleMatrix(const Vect4D& scale) {
	//	{	sx		0		0		0	}
	//	{	0		sy		0		0	}
	//	{	0		0		sz		0	}
	//	{	0		0		0		1	}
	m0 = scale.x;
	m5 = scale.y;
	m10 = scale.z;
}

void Matrix::setRotZMatrix(const float az) {
	//	{	cos		-sin	0		0	}
	//	{	sin		cos		0		0	}
	//	{	0		0		1		0	}
	//	{	0		0		0		1	}
	
	float sn = sin(az);
	float cs = cos(az);

	row0 = _mm_setr_ps(cs, -sn, 0.0f, 0.0f);
	row1 = _mm_setr_ps(sn, cs, 0.0f, 0.0f);
}

// End of file