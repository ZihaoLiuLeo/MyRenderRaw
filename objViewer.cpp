#include <vector>
#include <memory>
#include <algorithm>
#include "../GraphicLib/GraphicLib.h"
#include "../objProcessor/objProcessor.h"
#include "../ImageProcessor/TGAProcessor.h"

using Optr = std::shared_ptr<objPcs>;

using Tptr = std::shared_ptr<TGAProcessor>;

Optr objpcs(new objPcs("../obj/african_head.obj"));
Tptr tgapcs(new TGAProcessor(800, 800, TGAProcessor::RGB));

const int width = 800;
const int height = 800;

Vector3f eye(1, 1, 3);
Vector3f center(0, 0, 0);
Vector3f up(0, 1, 0);
Vector3f light_dir(1, 1, 1);

class GouraudShader : public IShader {
public:
	Vector3f vary_intensity;
	virtual Matrix vertex(int iface, int n_tri_vertex) override {
		vary_intensity[n_tri_vertex] = std::max(0.f, objpcs->getNorm(iface, n_tri_vertex)*light_dir);
		Matrix current_vertex(objpcs->getVertex(iface, n_tri_vertex)); // use assignment?
		return transform * current_vertex;
	}
	virtual bool fragment(Vector3f bar, TGAColor &color) override {
		float intensity = vary_intensity * bar;
		color = TGAColor(255, 255, 255)*intensity;
		return false;
	}
};

#ifdef TEST_OBJVIEWER

int main() {
	std::cout << "no" << std::endl;
	objpcs->loadObj();
	////Tptr zbuffer(new TGAProcessor(800, 800, TGAProcessor::GRAYSCALE));
	lookat(eye, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (eye - center).norm());
	light_dir.normalize();

	int* zbuffer = new int[width*height];
	for (int i = 0; i < width*height; i++) { zbuffer[i] = INT_MIN; }

	GouraudShader shader;
	shader.calcTransform();

	for (int i = 0; i < objpcs->getFaceNumber(); i++) {
		Vector3f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, *tgapcs, zbuffer, width);
	}

	std::cout << "finished writing." << std::endl;
	tgapcs->flip_vertically();
	tgapcs->write_tga_file("../output/african_head_refactored.tga");
	return 0;
}

#endif // TEST_OBJVIEWER