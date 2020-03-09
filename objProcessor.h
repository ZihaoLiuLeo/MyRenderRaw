#ifndef __OBJPROCESSOR_H_
#define __OBJPROCESSOR_H_
#include <vector>
#include "../Geometry/geometry.h"

class objPcs {
public:
	objPcs() = default;
	objPcs(const char *filename);
	objPcs(const objPcs& objpcs):
		vertex_(objpcs.vertex_),
		face_(objpcs.face_),
		texcoord_(objpcs.texcoord_),
		normal_(objpcs.normal_)
	{}

	bool loadObj();
	int getFaceNumber();
	Vector3f getVertex(int i);
	Vector3f getTex(int i);
	Vector3f getNorm(int iface, int ivertex);
	std::vector<int> face(int i);
	std::vector<int> uv(int iface, int ivertex);

	~objPcs();
private:
	std::vector<Vector3f> vertex_;
	// face: vertex/uv/normal
	std::vector<std::vector<Vector3i>> face_;
	std::vector<std::vector<int>> uv_;
	std::vector<Vector3f> texcoord_;
	std::vector<Vector3f> normal_;
	std::ifstream ifs;
};

#endif // !__OBJPRCESSOR_H_

