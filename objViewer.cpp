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
	Matrix vary_uv{ 2,3 };

	virtual Matrix vertex(int iface, int n_tri_vertex) override {
		vary_intensity[n_tri_vertex] = std::max(0.f, objpcs->getNorm(iface, n_tri_vertex)*light_dir);
		for (int i = 0; i < 2; i++) {
			vary_uv[i][n_tri_vertex] = (i == 0) ? objpcs->uv(iface, n_tri_vertex).x : objpcs->uv(iface, n_tri_vertex).y;
		}
		Matrix current_vertex(objpcs->getVertex(iface, n_tri_vertex)); // use assignment?
		return transform * current_vertex;
	}
	virtual bool fragment(Vector3f bar, TGAColor &color) override {
		float intensity = vary_intensity * bar;
		Vector3f temp1(vary_uv[0][0], vary_uv[0][1], vary_uv[0][2]);
		Vector3f temp2(vary_uv[1][0], vary_uv[1][1], vary_uv[1][2]);
		Vector2f uv(temp1 * bar, temp2 * bar); //TODO that is why it's necessary to make vector and matrix more flexible
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
	Matrix vary_uv{ 2,3 };
	Matrix uniform_M;
	Matrix uniform_MIT;

	virtual Matrix vertex(int iface, int n_tri_vertex) override {
		for (int i = 0; i < 2; i++) {
			vary_uv[i][n_tri_vertex] = (i == 0) ? objpcs->uv(iface, n_tri_vertex).x : objpcs->uv(iface, n_tri_vertex).y;
		}
		Matrix current_vertex(objpcs->getVertex(iface, n_tri_vertex)); // use assignment?
		return transform * current_vertex;
	}

	virtual bool fragment(Vector3f bar, TGAColor &color) override {
		Vector3f temp1(vary_uv[0][0], vary_uv[0][1], vary_uv[0][2]);
		Vector3f temp2(vary_uv[1][0], vary_uv[1][1], vary_uv[1][2]);
		Vector2f uv(temp1 * bar, temp2 * bar);
		Vector3f normal, light;
		Matrix n{ 4,1 }, l{ 4,1 };
		//std::cout << uv << std::endl;
		n = uniform_MIT * homorize(objpcs->normal(uv));
		l = uniform_M * homorize(light_dir);
		
		for (int i = 0; i < 3; i++)normal[i] = n[i][0];
		for (int i = 0; i < 3; i++)light[i] = l[i][0];
		
		float intensity = std::max(0.f, normal*light);
		//std::cout << intensity << std::endl;
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
	shader.uniform_MIT = (shader.uniform_M.inverse()).transpose();
	//std::cout << shader.uniform_M << std::endl;
	//std::cout << shader.uniform_MIT << std::endl;
	//int count = 0;
	for (int i = 0; i < objpcs->getFaceNumber(); i++) {
		//count++;
		//std::cout << count << std::endl;
		Vector3f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, *tgapcs, zbuffer, width);
	}

	std::cout << "finished writing." << std::endl;
	tgapcs->flip_vertically();
	std::cout << "yes" << std::endl;
	tgapcs->write_tga_file("../output/african_head_tex_with__normalmap.tga");
	return 0;
}

#endif // TEST_OBJVIEWER