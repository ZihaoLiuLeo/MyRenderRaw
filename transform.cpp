#include "transform.h"
#include <iostream>


#ifdef TEST_TRANSFORM
int main() {
	matrix44 ma = initMat<matrix44, 4, 1>();
	ma[0][0] = -1;
	ma[0][2] = 4;
	ma[2][3] = -5;
	ma[3][0] = -3;
	ma[2][2] = 4;
	//ma.invert_transpose();
	//std::cout << ma << std::endl;
	//ma.invert_transpose();
	//std::cout << ma << std::endl;
	vector4f i;
	std::cout << ma << std::endl;
	std::cout << ma.invert_transpose() << std::endl;
	Matrix<2, 2, float> m;
	m[0][0] = 1;
	m[0][1] = 2;
	m[1][0] = 3;
	m[1][1] = 4;
	std::cout << m << std::endl;
	std::cout << m.invert_transpose() << std::endl;
}
#endif