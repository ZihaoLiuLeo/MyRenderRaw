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
			std::vector<int> f;
			std::vector<int> uv;
			int idx, iuv, itrash;
			iss >> trash;
			while (iss >> idx >> trash >> iuv >> trash >> itrash) {
				idx--;
				iuv--;
				f.push_back(idx);
				uv.push_back(iuv);
			}
			face_.push_back(f);
			uv_.push_back(uv);
		}
	}
	return true;
}

int objPcs::getFaceNumber() {
	return face_.size();
}

Vector3f objPcs::getVertex(int i) {
	assert(i < vertex_.size());
	return vertex_[i];
}

Vector3f objPcs::getTex(int i) {
	assert(i < texcoord_.size());
	return texcoord_[i];
}

Vector3f objPcs::getNorm(int i) {
	assert(i < normal_.size());
	return normal_[i];
}

std::vector<int> objPcs::face(int i) {
	assert(i < face_.size());
	return face_[i];
}

std::vector<int> objPcs::uv(int i) {
	assert(i < uv_.size());
	return uv_[i];
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