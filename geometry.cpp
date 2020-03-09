#include <iostream>
#include <cassert>
#include "geometry.h"

template<>
Vector3<float>::Vector3(Matrix m) : x(m[0][0] / m[3][0]), y(m[1][0] / m[3][0]), z(m[2][0] / m[3][0]) {}
template<> template<>
Vector3<int>::Vector3(const Vector3<float>& v):x(int(v.x + .5)), y(int(v.y + .5)), z(int(v.z + .5)) {}
template<> template<>
Vector3<float>::Vector3(const Vector3<int>& v) : x(v.x), y(v.y), z(v.z) {}

Matrix::Matrix(Vector3f v) :m(std::vector<std::vector<float>>(4, std::vector<float>(1, 1.f))), rows(4), cols(1) {
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
}

int Matrix::row_size() {
	return rows;
}

int Matrix::col_size() {
	return cols;
}

Matrix Matrix::identity(int dimensions) {
	Matrix E(dimensions, dimensions);
	for (int i = 0; i < dimensions; i++) {
		for (int j = 0; j < dimensions; j++) {
			E[i][j] = (i == j ? 1.f : 0.f);
		}
	}
	return E;
}

std::vector<float>& Matrix::operator[](const int i) {
	assert(i >= 0 && i < rows);
	return m[i];
}

Matrix Matrix::operator*(const Matrix& a) {
	assert(cols == a.rows);
	Matrix ret(rows, a.cols);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < a.cols; j++) {
			ret.m[i][j] = 0.f;
			for (int k = 0; k < cols; k++) {
				ret.m[i][j] += m[i][k] * a.m[k][j];
			}
		}
	}
	return ret;
}

Matrix Matrix::transpose() {
	Matrix ret(cols, rows);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			ret[j][i] = m[i][j];
		}
	}
	return ret;
}

Matrix Matrix::inverse() {
	assert(rows == cols);
	Matrix ret(rows, cols * 2);
	// initialize A|E
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			ret[i][j] = m[i][j];
		}
	}
	for (int i = 0; i < rows; i++) {
		ret[i][i + cols] = 1;
	}
	// add first to the second
	for (int i = 0; i < rows - 1; i++) {
		// normalize the first row
		for (int j = ret.cols - 1; j >= 0; j--) {
			ret[i][j] /= ret[i][i];
		}
		for (int k = i + 1; k < rows; k++) {
			float coeff = ret[k][i];
			for (int j = 0; j < ret.cols; j++) {
				ret[k][j] -= ret[i][j] * coeff;
			}
		}
	}

	// normalize the last row
	for (int j = ret.cols - 1; j >= rows - 1; j--) {
		ret[rows - 1][j] /= ret[rows - 1][rows - 1];
	}
	// add second to the first
	for (int i = rows - 1; i > 0; i--) {
		for (int k = i - 1; k >= 0; k--) {
			float coeff = ret[k][i];
			for (int j = 0; j < ret.cols; j++) {
				ret[k][j] -= ret[i][j] * coeff;
			}
		}
	}
	// cut the identity matrix
	Matrix truncate(rows, cols);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			truncate[i][j] = ret[i][j + cols];
		}
	}
	return truncate;
}


#ifdef TEST_GEO
int main()
{
	Vector3i three(1, 1, 1);
	Vector3i three2(1, 2, 3);
	Vector3i more = three + three2;
	three += three2;
	Vector2i two(1, 1);
	std::cout << three << std::endl;
	std::cout << two << std::endl;
	std::cout << more << std::endl;
	std::cout << three[2] << std::endl;
	return 0;
}
#endif