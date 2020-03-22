#ifndef __OBJPROCESSOR_H_
#define __OBJPROCESSOR_H_
#include <vector>
#include "../Geometry2/transform.h"
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
	vector3f getVertex(int i);
	vector3f getVertex(int iface, int n_tri_vertex);
	vector2f getTex(int i);
	vector3f getNorm(int iface, int ivertex);
	std::vector<int> face(int i);
	vector2f uv(int iface, int ivertex);
	void load_texture(TGAProcessor *texture);
	void load_normal(TGAProcessor *normal);
	void load_specular(TGAProcessor *spec);
	TGAColor diffuse(vector2f uv);
	vector3f normal(vector2f uv);
	float specular(vector2f uv);

	~objPcs();
private:
	std::vector<vector3f> vertex_;
	// face: vertex/uv/normal
	std::vector<std::vector<vector3i>> face_;
	std::vector<vector2f> texcoord_;
	std::vector<vector3f> normal_;
	std::ifstream ifs;
	TGAProcessor *diffusemap_;
	TGAProcessor *normalmap_;
	TGAProcessor *specularmap_;
};

#endif // !__OBJPRCESSOR_H_

