#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include "objProcessor.h"


objPcs::objPcs(const char *filename) {
	ifs = std::ifstream(filename);
	std::stringstream errss;
	if (!ifs) {
		errss << "cannot open file " << filename << std::endl;
		return;
	}
}

bool objPcs::getObjFile(const char* filename) {
	ifs = std::ifstream(filename);
	std::stringstream errss;
	if (!ifs) {
		errss << "cannot open file " << filename << std::endl;
		return false;
	}
	return true;
}

bool objPcs::loadObj() {
	while (ifs.peek() != -1) {
		std::string linebuf;
		std::getline(ifs, linebuf);
		std::istringstream iss(linebuf.c_str());
		char trash;
		if (!linebuf.compare(0, 2, "v ")) {
			iss >> trash;
			vector3f v;
			for (int i = 0; i < 3; i++) { iss >> v[i]; }
			vertex_.push_back(v);
		}
		else if (!linebuf.compare(0, 2, "vt")) {
			char trash2;
			iss >> trash >> trash2;
			vector2f v;
			for (int i = 0; i < 2; i++) { iss >> v[i];}
			texcoord_.push_back(v);
		}
		else if (!linebuf.compare(0, 2, "vn")) {
			char trash2;
			iss >> trash >> trash2;
			vector3f v;
			for (int i = 0; i < 3; i++) { iss >> v[i]; }
			normal_.push_back(v);
		}
		else if (!linebuf.compare(0, 1, "f")) {
			std::vector<vector3i> f;
			vector3i temp;
			iss >> trash;
			//TODO make Vector dimension to get vector size
			while (iss >> temp[0] >> trash >> temp[1] >> trash >> temp[2]) {
				for (int i = 0; i < 3; i++)temp[i]--;
				f.push_back(temp);
			}
			face_.push_back(f);
		}
	}
	std::cout << "Total vertices: " << vertex_.size() << std::endl;
	std::cout << "Total faces: " << face_.size() << std::endl;
	return true;
}

int objPcs::getFaceNumber() {
	return face_.size();
}

vector3f objPcs::getVertex(int i) {
	assert(("vertex out of bound. ",i < vertex_.size()));
	return vertex_[i];
}

vector3f objPcs::getVertex(int iface, int n_tri_vertex) {
	assert("face out of bound. ", iface < face_.size());
	assert("triangle vertex index out of bound.", n_tri_vertex < 3);
	return getVertex(face_[iface][n_tri_vertex][0]);
}

vector3f objPcs::getNorm(int iface, int ivertex) {
	assert(("face size out of bound. ", iface < face_.size()));
	assert(("vertex size out of bound. ", ivertex < vertex_.size()));
	int idx = face_[iface][ivertex][2];
	return normal_[idx].normalize();
}

std::vector<int> objPcs::face(int idx) {
	assert(("face size out of bound. ", idx < face_.size()));
	std::vector<int> face;
	for (int i = 0; i < face_[idx].size(); i++)face.push_back(face_[idx][i][0]);
	return face;
}

vector2f objPcs::getTex(int i) {
	assert(("tex coords out of bound. ", i < texcoord_.size()));
	return texcoord_[i];
}

vector2f objPcs::uv(int iface, int ivertex) {
	assert(("face size out of bound. ",iface < face_.size()));
	assert(("vertex size out of bound. ",ivertex < vertex_.size()));
	int idx = face_[iface][ivertex][1];
	return getTex(idx);
}

void objPcs::load_texture(TGAProcessor *texture) {
	diffusemap_ = texture;
	diffusemap_->flip_vertically();
}

TGAColor objPcs::diffuse(vector2f uv) {
	vector2i uvn(uv[0] * diffusemap_->get_width(), uv[1] * diffusemap_->get_height());
	return diffusemap_->get(uvn[0], uvn[1]);
}

void objPcs::load_normal(TGAProcessor *normal) {
	normalmap_ = normal;
	normalmap_->flip_vertically();
}

vector3f objPcs::normal(vector2f uv){
	vector2i uvn(uv[0] * normalmap_->get_width(), uv[1] * normalmap_->get_height());
	TGAColor c = normalmap_->get(uvn[0], uvn[1]);
	vector3f res;
	for (int i = 0; i < 3; i++)
		res[2 - i] = (float)c[i] / 255.f*2.f - 1.f; // retrieve info from normal map
	return res;
}
objPcs::~objPcs() {
	delete[] diffusemap_;
}

#ifdef TEST_OBJPCS
int main()
{
	objPcs pcs("../obj/african_head.obj");
	pcs.loadObj();
	std::cout << "Finished." << std::endl;
}
#endif