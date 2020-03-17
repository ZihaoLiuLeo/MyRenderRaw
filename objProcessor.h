#ifndef __OBJPROCESSOR_H_
#define __OBJPROCESSOR_H_
#include <vector>
#include "../Geometry/geometry.h"
#include "../ImageProcessor/TGAProcessor.h"

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

	bool getObjFile(const char* filename);
	bool loadObj();
	int getFaceNumber();
	Vector3f getVertex(int i);
	Vector3f getVertex(int iface, int n_tri_vertex);
	Vector2f getTex(int i);
	Vector3f getNorm(int iface, int ivertex);
	std::vector<int> face(int i);
	Vector2f uv(int iface, int ivertex);
	void load_texture(TGAProcessor *texture);
	void load_normal(TGAProcessor *normal);
	TGAColor diffuse(Vector2f uv);
	Vector3f normal(Vector2f uv);

	~objPcs();
private:
	std::vector<Vector3f> vertex_;
	// face: vertex/uv/normal
	std::vector<std::vector<Vector3i>> face_;
	std::vector<Vector2f> texcoord_;
	std::vector<Vector3f> normal_;
	std::ifstream ifs;
	TGAProcessor *diffusemap_;
	TGAProcessor *normalmap_;
};

#endif // !__OBJPRCESSOR_H_

