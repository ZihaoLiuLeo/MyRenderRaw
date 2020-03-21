#include <vector>
#include <memory>
#include <algorithm>
#include "../GraphicLib/GraphicLib.h"
#include "../objProcessor/objProcessor.h"
#include "../ImageProcessor/TGAProcessor.h"
#include "../Geometry2/transform.h"

using Optr = std::shared_ptr<objPcs>;

using Tptr = std::shared_ptr<TGAProcessor>;

Optr objpcs(new objPcs("../obj/african_head.obj"));
Tptr tgapcs(new TGAProcessor(800, 800, TGAProcessor::RGB));


const int width = 800;
const int height = 800;

vector3f eye(1, 1, 3);
vector3f center(0, 0, 0);
vector3f up(0, 1, 0);
vector3f light_dir(1, 1, 1);

class GouraudShader : public IShader {
public:
	vector3f vary_intensity;
	Matrix<2,3, float> vary_uv;

	virtual vector4f vertex(int iface, int n_tri_vertex) override {
		vary_intensity[n_tri_vertex] = std::max(0.f, objpcs->getNorm(iface, n_tri_vertex)*light_dir);
		for (int i = 0; i < 2; i++) {
			vary_uv[i][n_tri_vertex] = (i == 0) ? objpcs->uv(iface, n_tri_vertex).x : objpcs->uv(iface, n_tri_vertex).y;
		}
		vector4f current_vertex =  homorize<4>(objpcs->getVertex(iface, n_tri_vertex)); // use assignment?
		return transform * current_vertex;
	}
	virtual bool fragment(vector3f bar, TGAColor &color) override {
		float intensity = vary_intensity * bar;
		vector3f temp1(vary_uv[0][0], vary_uv[0][1], vary_uv[0][2]);
		vector3f temp2(vary_uv[1][0], vary_uv[1][1], vary_uv[1][2]);
		vector2f uv(temp1 * bar, temp2 * bar); //TODO that is why it's necessary to make vector and matrix more flexible
		//std::cout << uv << std::endl;
		/*if (intensity > .85) intensity = 1;
		else if (intensity > .60) intensity = .80;
		else if (intensity > .45) intensity = .60;
		else if (intensity > .30) intensity = .45;
		else if (intensity > .15) intensity = .30;
		else intensity = 0;
		color = TGAColor(255, 155, 0)*intensity;*/
		color = objpcs->diffuse(uv)*intensity;
		return false;
	}
};

// Shader using normal map
// So we dont need to interpolate normal value
class Shader :public IShader {
public:
	Matrix<2,3,float> vary_uv;
	matrix44 uniform_M;
	matrix44 uniform_MIT;

	virtual vector4f vertex(int iface, int n_tri_vertex) override {
		vary_uv.set_col(n_tri_vertex, objpcs->uv(iface, n_tri_vertex));
		// in previous version, w in matrix has been filled with 0, in the new versiion, w is filled with 1
		// TODO need to debug here or in triangle function
		vector4f current_vertex = homorize<4>(objpcs->getVertex(iface, n_tri_vertex));
		return transform * current_vertex;
	}

	virtual bool fragment(vector3f bar, TGAColor &color) override {
		vector2f uv = vary_uv * bar;
		vector3f normal, light;
		normal = proj<3>(uniform_MIT * homorize<4>(objpcs->normal(uv))).normalize();
		light = proj<3>(uniform_M * homorize<4>(light_dir)).normalize();

		float intensity = std::max(0.f, normal*light);
		color = objpcs->diffuse(uv)*intensity;
		return false;
	}
};

#ifdef TEST_OBJVIEWER

int main() {
	TGAProcessor* texpcs = new TGAProcessor();
	TGAProcessor* normpcs = new TGAProcessor();
	objpcs->loadObj();
	texpcs->read_tga_file("../obj/african_head_diffuse.tga");
	normpcs->read_tga_file("../obj/african_head_nm.tga");
	
	objpcs->load_texture(texpcs);
	objpcs->load_normal(normpcs);

	lookat(eye, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (eye - center).norm());
	light_dir.normalize();

	int* zbuffer = new int[width*height];
	for (int i = 0; i < width*height; i++) { zbuffer[i] = INT_MIN; }
	
	Shader shader;
	shader.calcTransform();
	shader.uniform_M = Projection * ModelView;
	shader.uniform_MIT = shader.uniform_M.invert_transpose();
	for (int i = 0; i < objpcs->getFaceNumber(); i++) {
		vector4f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, *tgapcs, zbuffer, width);
	}

	std::cout << "finished writing." << std::endl;
	tgapcs->flip_vertically();
	tgapcs->write_tga_file("../output/african_head_new.tga");
	return 0;
}

#endif // TEST_OBJVIEWER