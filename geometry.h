#ifndef __GEOMETRY_H_
#define __GEOMETRY_H_
#include <iostream>
#include <cmath>
#include <vector>

class Matrix;

template<typename T>
class Vector2{
public:
	Vector2<T>():x(T()), y(T()){}
	Vector2<T>(T xx, T yy) :x(xx), y(yy) {}
	
	Vector2<T> operator+(const Vector2<T> &v) const{
		return Vector2<T>(x + v.x, y + v.y);
	}
	Vector2<T> operator-(const Vector2<T> &v) const{
		return Vector2<T>(x - v.x, y - v.y);
	}
	template<typename U>
	Vector2<T> operator*(U f) const{
		return Vector2<T>(f*x, f*y);
	}

	T &operator[](int i) const {
		if (i == 0) return x;
		return y;
	}
	T &operator[](int i) {
		if (i == 0)return x;
		return y;
	}
	T x, y;
};

template<typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector2<T> &v) {
	os << "[ " << v.x << ", " << v.y << " ]";
	return os;
}

template<>
inline std::ostream &operator<<(std::ostream &os, const Vector2<float> &v) {
	os << std::printf("[ %f, %f]", v.x, v.y);
	return os;
}

template<typename T>
class Vector3{
public:
	T x, y, z;
	Vector3<T>():x(T()),y(T()),z(T()) {}
	Vector3<T>(T xx, T yy, T zz) :x(xx), y(yy), z(zz) {}
	Vector3<T>(Matrix m);
	template<class U> Vector3<T>(const Vector3<U> &v);

	inline Vector3<T> operator+(const Vector3<T> &v) const {
		return Vector3<T>(x + v.x, y + v.y, z + v.z);
	}
	inline Vector3<T> &operator+=(const Vector3<T> &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	inline Vector3<T> operator-(const Vector3<T> &v) const {
		return Vector3<T>(x - v.x, y - v.y, z - v.z);
	}
	inline Vector3<T> operator^(const Vector3<T> v)const {
		return Vector3<T>(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}

	template<typename U>
	inline Vector3<T> operator*(U f) const {
		return Vector3<T>(f*x, f*y, f*z);
	}
	inline T operator*(const Vector3<T> &v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	inline T &operator[](int i) const {
		if (i == 0) return x;
		else if (i == 1) return y;
		else return z;
	}
	inline T &operator[](int i) {
		if (i == 0)return x;
		else if (i == 1)return y;
		else return z;
	}
	
	inline float norm() const { return std::sqrt(x*x + y * y + z * z); }
	inline Vector3<T>& normalize(T l=1) {*this = (*this)*(l/norm()); return *this; }
};

template<typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector3<T> &v) {
	os << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";
	return os;
}

template<>
inline std::ostream &operator<<(std::ostream &os, const Vector3<float> &v) {
	os << std::printf("[ %f, %f, %f]", v.x, v.y, v.z);
	return os;
}

typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;

template<> template<>
Vector3<int>::Vector3(const Vector3<float> &v);
template<> template<>
Vector3<float>::Vector3(const Vector3<int> &v);

template<typename T>
inline Vector3<T> cross(Vector3<T> v1, Vector3<T> v2) {
	return Vector3<T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

//-----------<Matrix class>---------------//
class Matrix {
public:
	Matrix(int r = 4, int c = 4);
	Matrix(Vector3f v);
	int row_size();
	int col_size();
	static Matrix identity(int dimensions);
	std::vector<float> &operator[](const int i);
	Matrix operator*(const Matrix& a);
	Matrix transpose();
	Matrix inverse();

	std::vector<std::vector<float>> m;
	int rows, cols;
};

inline std::ostream &operator<<(std::ostream &os, Matrix& m) {
	for (int i = 0; i < m.rows; i++) {
		for (int j = 0; j < m.cols; j++) {
			os << m[i][j];
			if (i < m.col_size()) os << "\t";
		}
		os << std::endl;
	}
	return os;
}

inline Matrix homorize(Vector3f v) {
	Matrix res{ 4,1 };
	for (int i = 0; i < 3; i++) res[i][0] = v[i];
	res[3][0] = 1;
	return res;
}

#endif // !__GEOMETRY_H_
