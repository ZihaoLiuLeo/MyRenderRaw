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

bool objPcs::loadObj() {
	while (ifs.peek() != -1) {
		std::string linebuf;
		std::getline(ifs, linebuf);
		std::istringstream iss(linebuf.c_str());
		char trash;
		if (!linebuf.compare(0, 2, "v ")) {
			iss >> trash;
			Vector3f v;
			for (int i = 0; i < 3; i++) { iss >> v[i]; }
			vertex_.push_back(v);
		}
		else if (!linebuf.compare(0, 2, "vt")) {
			char trash2;
			iss >> trash >> trash2;
			Vector3f v;
			for (int i = 0; i < 3; i++) { iss >> v[i];}
			texcoord_.push_back(v);
		}
		else if (!linebuf.compare(0, 2, "vn")) {
			char trash2;
			iss >> trash >> trash2;
			Vector3f v;
			for (int i = 0; i < 3; i++) { iss >> v[i]; }
			normal_.push_back(v);
		}
		else if (!linebuf.compare(0, 1, "f")) {
			std::vector<Vector3i> f;
			Vector3i temp;
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

Vector3f objPcs::getVertex(int i) {
	assert(("vertex out of bound. ",i < vertex_.size()));
	return vertex_[i];
}

Vector3f objPcs::getTex(int i) {
	assert(("tex coords out of bound. ", i < texcoord_.size()));
	return texcoord_[i];
}

Vector3f objPcs::getNorm(int iface, int ivertex) {
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

//TODO load texture
std::vector<int> objPcs::uv(int iface, int ivertex) {
	assert(("face size out of bound. ",iface < face_.size()));
	assert(("vertex size out of bound. ",ivertex < vertex_.size()));
	int idx = face_[iface][ivertex][1];
	return uv_[0];
}

objPcs::~objPcs() {
}

#ifdef TEST_OBJPCS
int main()
{
	objPcs pcs("../obj/african_head.obj");
	pcs.loadObj();
	std::cout << "Finished." << std::endl;
}
#endif