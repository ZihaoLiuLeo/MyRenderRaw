#include <iostream>
#include "geometry.h"


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