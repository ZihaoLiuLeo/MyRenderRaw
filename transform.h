////////////////////////////////////////////////////////////////////////////////////////
//Zihao Liu                                                                           //
//1.0 version                                                                         //
//2020/3/20                                                                           //
//Geometry for render                                                                 //
//Using metaprogramming to implement basic matrix transforms.                         //
//original code from:                                                                 //
//https://github.com/ssloy/tinyrenderer/tree/f037c7a0517a632c7391b35131f9746a8f8bb235 //
//Metaprogramming algorithm from: Pete Isensee (PKIsensee@msn.com).                   //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <iostream>
#include <cassert>

// generic vector
template <size_t D, typename T>
struct Vector {
	Vector() { for (size_t i = D; i--; data_[i] = T()); }
	T& operator[](const size_t i) { assert(i < D); return data_[i]; }
	const T& operator[](const size_t i) const { assert(i < D); return data_[i]; }
private:
	T data_[D];
};

// specialized Vector2
template <typename T>
struct Vector<2, T> {
	Vector() :x(T()), y(T()) {}
	Vector(T X, T Y) :x(X), y(Y) {}

	template<class U>
	Vector<2, T>(const Vector<2, U> &v);

	T& operator[](const size_t i) { assert(i < 2); return i <= 0 ? x : y; }
	const T& operator[](const size_t i) const { assert(i < 2); return i <= 0 ? x : y; }

	T x, y;
};

// speialized Vector3
template <typename T>
struct Vector<3, T> {
	Vector() :x(T()), y(T()), z(T()) {}
	Vector(T X, T Y, T Z) :x(X), y(Y), z(Z) {}

	template<class U>
	Vector<3, T>(const Vector<3, U> &v);

	T& operator[](const size_t i) { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
	const T& operator[](const size_t i) const { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }

	float norm() { return std::sqrt(x * x + y * y + z * z); }
	Vector<3, T>& normalize(T l = 1) { *this = (*this)*(l / norm()); return *this; }

	T x, y, z;
};

// operations
template<size_t D, typename T>
inline T operator*(const Vector<D, T>& v1, const Vector<D, T>& v2) {
	T ret = T();
	for (size_t i = D; i--; ret += v1[i] * v2[i]);
	return ret;
}

template<size_t D, typename T>
inline Vector<D, T> operator-(Vector<D, T> v1, const Vector<D, T>& v2) {
	for (size_t i = D; i--; v1[i] -= v2[i]);
	return v1;
}

template<size_t D, typename T>
Vector<D, T> operator+(Vector<D, T> v1, const Vector<D, T>& v2) {
	for (size_t i = D; i--; v1[i] += v2[i]);
	return v1;
}

template<size_t D, typename T, typename U>
inline Vector<D, T> operator*(Vector<D, T> v1, const U& v2) {
	for (size_t i = D; i--; v1[i] *= v2);
	return v1;
}

template<size_t D, typename T, typename U>
inline Vector<D, T> operator/(Vector<D, T> v1, const U& v2) {
	for (size_t i = D; i--; v1[i] /= v2);
	return v1;
}

template<size_t D, typename T>
inline bool operator==(const Vector<D, T>& v1, const Vector<D, T>& v2) {
	for (size_t i = 0; i < D; i++) {
		if (v1[i] != v2[i]) { return false; }
	}
	return true;
}

// fill homo with 1 or 0
template<size_t L, size_t D, typename T>
inline Vector<L, T> homorize(const Vector<D, T>& v, T fill = 1) {
	Vector<L, T> ret;
	for (size_t i = L; i--; ret[i] = (i < D ? v[i] : fill));
	return ret;
}

template<size_t L, size_t D, typename T>
inline Vector<L, T> proj(const Vector<D, T>& v) {
	Vector<L, T> ret;
	for (size_t i = L; i--; ret[i] = v[i]);
	return ret;
}

template<typename T>
inline Vector<3, T> cross(const Vector<3, T>& v1, const Vector<3, T>& v2) {
	return Vector<3, T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

template<size_t D, typename T>
inline std::ostream& operator<<(std::ostream& os, Vector<D, T>& v) {
	for (int i = 0; i < D; i++) {
		os << v[i] << " ";
	}
	return os;
}

typedef Vector<2, int> vector2i;
typedef Vector<2, float> vector2f;
typedef Vector<3, int> vector3i;
typedef Vector<3, float> vector3f;
typedef Vector<4, int> vector4i;
typedef Vector<4, float> vector4f;

/////////////////////////////////////////////
//-----<cross by meta programming>---------//
/////////////////////////////////////////////

template<class Vec, int N>
struct CrossVec {
	static inline void eval(Vec& vec, const Vec& v1, const Vec& v2) {
		CrossVecImplement<Vec, N, 0>::eval(vec, v1, v2);
	}
};

template<class Vec, int N, int I>
struct CrossVecImplement {
	enum {
		NextI = I + 1,
		Index1 = NextI % N,
		Index2 = (NextI + 1) % N
	};
	static inline void eval(Vec& vec, const Vec& v1, const Vec& v2) {
		vec[I] = v1[Index1] * v2[Index2] - v1[Index2] * v2[Index1];
		CrossVecImplement<Vec, N, NextI>::eval(vec, v1, v2);
	}
};

template<>
struct CrossVecImplement <vector3f, 3, 3> {
	static inline void eval(vector3f&, const vector3f&, const vector3f&) {}
};

#define CrossVec(VecType, ret, v1, v2, N) CrossVec<VecType, N>::eval(ret, v1, v2);

// cross override
template<>
inline vector3f cross(const vector3f& v1, const vector3f& v2) {
	vector3f ret;
	CrossVec(vector3f, ret, v1, v2, 3);
	return ret;
}

/////////////////////////////////////////////////
//<---------><matrix implementation><--------->//
/////////////////////////////////////////////////


template<size_t R, size_t C, typename T>
struct Matrix {
	Vector<C, T> rows[R];
public:
	Matrix() {}

	Matrix<C, R, T>& transpose();

	T det() const;
	T cofactor(size_t row, size_t col) const;
	Matrix<R, C, T> adjugate() const;
	Matrix<R - 1, C - 1, T> minor(size_t row, size_t col) const;
	Matrix<R, C, T> invert_transpose() const;
	Matrix<R, C, T> invert() const;

	inline Vector<C, T>& operator[](const size_t idx) {
		assert(idx < R);
		return rows[idx];
	}

	inline const Vector<C, T>& operator[] (const size_t idx) const {
		assert(idx < R);
		return rows[idx];
	}

	inline Vector<R, T> col(const size_t idx) const {
		assert(idx < C);
		Vector<R, T> ret;
		for (size_t i = R; i--; ret[i] = rows[i][idx]);
		return ret;
	}

	inline void set_col(size_t idx, Vector<R, T> v) {
		assert(idx < C);
		for (size_t i = R; i--; rows[i][idx] = v[i]);
	}
};

///////////////////////////////////////
//---------<operator overload>-------//
///////////////////////////////////////

template<size_t R, size_t C, typename T>
inline Matrix<R, C, T> operator/(Matrix<R, C, T> mat, const T& f) {
	for (size_t i = R; i--; mat[i] = mat[i] / f);
	return mat;
}

template<size_t R, size_t C, typename T>
inline std::ostream& operator<<(std::ostream& os, Matrix<R, C, T> m) {
	for (size_t i = 0; i < R; i++) { os << "[ " << m[i] << " ]" << std::endl; }
	return os;
}

typedef Matrix<3, 3, float> matrix33;
typedef Matrix<4, 4, float> matrix44;

// template meta programming
// identity matrix
// Copyright © 2000 Pete Isensee (PKIsensee@msn.com).

// it has to be int, not size_t

///////////////////////////////////////////
//---------<identity matrix>-------------//
///////////////////////////////////////////

template<class Mat, int N>
struct IdMat {
	static inline void eval(Mat& mat) {
		IdMatImplement<Mat, N, 0, 0, 0>::eval(mat);
	}
};

template<class Mat, int N, int R, int C, int I>
struct IdMatImplement {
	enum {
		NextI = I + 1,
		NextC = NextI % N,
		NextR = NextI / N % N
	};
	static inline void eval(Mat& mat) {
		mat[R][C] = (C == R) ? 1.0f : 0.0f;
		IdMatImplement< Mat, N, NextR, NextC, NextI >::eval(mat);
	}
};

template<> struct IdMatImplement<matrix33, 3, 0, 0, 3 * 3> {
	static inline void eval(matrix33&) {}
};

template<> struct IdMatImplement<matrix44, 4, 0, 0, 4 * 4 > {
	static inline void eval(matrix44&) {}
};

#define IdentityMat(MatType, Mat, N) IdMat<MatType, N>::eval( Mat )

template<typename T, int N>
inline T identity() {
	T mat;
	IdentityMat(T, mat, N);
	return mat;
}

//////////////////////////////////////////
//------------<init matrix>-------------//
//////////////////////////////////////////

template<class Mat, int N, class F, int Init>
struct InitMat {
	static inline void eval(Mat& mat) {
		InitMatImplement<Mat, N, F, Init, 0, 0, 0>::eval(mat);
	}
};

template<class Mat, int N, class F, int Init, int R, int C, int I>
struct InitMatImplement {
	enum {
		NextI = I + 1,
		NextC = NextI % N,
		NextR = NextI / N % N
	};
	static inline void eval(Mat& mat) {
		mat[R][C] = static_cast<F>(Init);
		InitMatImplement<Mat, N, F, Init, NextR, NextC, NextI>::eval(mat);
	}
};

template<>
struct InitMatImplement<matrix33, 3, float, 0, 0, 0, 3 * 3> {
	static inline void eval(matrix33&) {}
};

template<>
struct InitMatImplement<matrix33, 3, float, 1, 0, 0, 3 * 3> {
	static inline void eval(matrix33&) {}
};

template<>
struct InitMatImplement<matrix44, 4, float, 0, 0, 0, 4 * 4> {
	static inline void eval(matrix44&) {}
};

template<>
struct InitMatImplement<matrix44, 4, float, 1, 0, 0, 4 * 4> {
	static inline void eval(matrix44&) {}
};

#define InitMat(MatType, Mat, N, F, Init) InitMat<MatType, N, F, Init>::eval(Mat)

// enter matrix type, dimension, initialize value
template<typename T, int N, int I>
inline T initMat() {
	assert(I == 1 || I == 0);
	T ma;
	InitMat(T, ma, N, float, I);
	return ma;
}

//////////////////////////////////
//---------<transpose>----------//
//////////////////////////////////
template<class Mat, int N>
struct TransMat {
	static inline void eval(Mat& mat) {
		TransMatImplement<Mat, N, 0, 1, 0>::eval(mat);
	}
};

template<class Mat, int N, int R, int C, int I>
struct TransMatImplement {
	enum {
		NextI = I + 1,
		NextR = NextI / N % N,
		NextC = (NextI % N) + NextR + 1
	};
	static inline void eval(Mat& mat) {
		if (C < N) std::swap(mat[R][C], mat[C][R]);
		TransMatImplement<Mat, N, NextR, NextC, NextI>::eval(mat);
	}
};

template<>
struct TransMatImplement<matrix33, 3, 0, 1, 3 * 3> {
	static inline void eval(matrix33&) {}
};

template<>
struct TransMatImplement<matrix44, 4, 0, 1, 4 * 4> {
	static inline void eval(matrix44&) {}
};

#define TransMat(MatType, Mat, N) TransMat<MatType, N>::eval(Mat);

template<typename T, int N>
T transposeMat(T mat) {
	TransMat(T, mat, N);
	return mat;
}

// override in the matrix class
template<size_t R, size_t C, typename T>
inline Matrix<C, R, T>& Matrix<R, C, T>::transpose() {
	Matrix<C, R, T> ret;
	for (size_t i = 0; i < C; i++) {
		ret[i] = this->col(i);
	}
	return ret;
}

template<>
inline matrix33& matrix33::transpose() {
	matrix33 ret = *this;
	TransMat(matrix33, ret, 3);
	return ret;
}

template<>
inline matrix44& matrix44::transpose() {
	matrix44 ret = *this;
	TransMat(matrix44, ret, 4);
	return ret;
}

/////////////////////////////////////////
//---------<multiply matrix>-----------//
/////////////////////////////////////////

template<class Mat, int N>
struct MultMat {
	static inline void eval(Mat& r, const Mat& a, const Mat& b) {
		InitMat(Mat, r, N, float, 0);
		MultMatImplement<Mat, N, 0, 0, 0, 0>::eval(r, a, b);
	}
};

template<class Mat, int N, int R, int C, int K, int I>
struct MultMatImplement {
	enum {
		NextI = I + 1,
		NextK = NextI % N,
		NextC = NextI / N % N,
		NextR = NextI / N / N % N
	};
	static inline void eval(Mat& r, const Mat& a, const Mat& b) {
		r[R][C] += a[R][K] * b[K][C];
		MultMatImplement<Mat, N, NextR, NextC, NextK, NextI>::eval(r, a, b);
	}
};

template<>
struct MultMatImplement<matrix33, 3, 0, 0, 0, 3 * 3 * 3> {
	static inline void eval(matrix33&, const matrix33&, const matrix33&) {}
};

template<>
struct MultMatImplement<matrix44, 4, 0, 0, 0, 4 * 4 * 4> {
	static inline void eval(matrix44&, const matrix44&, const matrix44&) {}
};

#define MultMat(MatType, r, a, b, N) MultMat<MatType, N>::eval(r,a,b)

template<size_t R, size_t C, typename T>
inline Vector<R, T> operator*(const Matrix<R, C, T>& m, const Vector<C, T>& v) {
	Vector<R, T> ret;
	for (size_t i = 0; i < R; i++) {
		ret[i] = m[i] * v;
	}
	return ret;
}

template<size_t R1, size_t C1, size_t C2, typename T>
inline Matrix<R1, C2, T> operator*(const Matrix<R1, C1, T>& m1, const Matrix<C1, C2, T>& m2) {
	Matrix<R1, C2, T> ret;
	for (size_t i = 0; i < R1; i++) {
		for (size_t j = 0; j < C2; j++) {
			ret[i][j] = m1[i] * m2.col(j);
		}
	}
	return ret;
}


inline matrix44 operator*(const matrix44& v1, const matrix44& v2) {
	matrix44 ret;
	InitMat(matrix44, ret, 4, float, 0);
	MultMat(matrix44, ret, v1, v2, 4);
	return ret;
}

inline matrix33 operator*(const matrix33& v1, const matrix33& v2) {
	matrix33 ret;
	InitMat(matrix33, ret, 3, float, 0);
	MultMat(matrix33, ret, v1, v2, 3);
	return ret;
}

/////////////////////////////////////////////
//---------<get inverse transpose>---------//
/////////////////////////////////////////////

// get minor matrix
template<size_t R, size_t C, typename T>
inline Matrix<R - 1, C - 1, T> Matrix<R, C, T>::minor(size_t row, size_t col) const {
	Matrix<R - 1, C - 1, T> ret;
	for (size_t i = 0; i < R - 1; i++) {
		for (size_t j = 0; j < C - 1; j++) {
			ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1];
		}
	}
	return ret;
}

// det using metaprogramming
template<size_t D, typename T>
struct dt {
	static T det(const Matrix<D, D, T>& mat) {
		T ret = 0;
		for (size_t i = D; i--; ret += mat[0][i] * mat.cofactor(0, i));
		return ret;
	}
};

template<typename T>
struct dt<1, T> {
	static T det(const Matrix<1, 1, T>& mat) {
		return mat[0][0];
	}
};

//#define GetDet(Dim, Type, Mat) dt<Dim, type>::det(Mat)

template<size_t R, size_t C, typename T>
inline T Matrix<R, C, T>::cofactor(size_t row, size_t col) const {
	return minor(row, col).det()*((row + col) % 2 ? -1 : 1);
}

template<size_t R, size_t C, typename T>
inline T Matrix<R, C, T>::det() const {
	//return GetDet(R, T, *this);
	return dt<R, T>::det(*this);
}

template<size_t R, size_t C, typename T>
inline Matrix<R, C, T> Matrix<R, C, T>::adjugate() const {
	Matrix<R, C, T> ret;
	for (size_t i = 0; i < R; i++) {
		for (size_t j = 0; j < C; j++) {
			ret[i][j] = cofactor(i, j);
		}
	}
	return ret;
}

template<size_t R, size_t C, typename T>
inline Matrix<R, C, T> Matrix<R, C, T>::invert_transpose() const {
	Matrix<R, C, T> ret = adjugate();
	T temp = ret[0] * rows[0];
	return ret / temp;
}

template<size_t R, size_t C, typename T>
inline Matrix<R, C, T> Matrix<R, C, T>::invert() const {
	return invert_transpose().transpose();
}

template <> template <>
inline Vector<3, int>::Vector(const Vector<3, float>& v) :x(int(v.x + .5f)), y(int(v.y + .5f)), z(int(v.z + .5f)) {}

template <> template <>
inline Vector<3, float>::Vector(const Vector<3, int>& v) : x(v.x), y(v.y), z(v.z) {}

template <> template <>
inline Vector<2, int>::Vector(const Vector<2, float>& v) : x(int(v.x + .5f)), y(int(v.y + .5f)) {}

template <> template <>
inline Vector<2, float>::Vector(const Vector<2, int> &v) : x(v.x), y(v.y) {}

#endif