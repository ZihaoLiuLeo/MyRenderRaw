#include <vector>
#include <memory>
#include <algorithm>
#include "../GraphicLib/GraphicLib.h"
#include "../objProcessor/objProcessor.h"
#include "../ImageProcessor/TGAProcessor.h"
#include "../Geometry2/transform.h"

using Optr = std::shared_ptr<objPcs>;
using Tptr = std::shared_ptr<TGAProcessor>;

Optr objpcs(new objPcs("../obj/diablo3_pose.obj"));
Tptr tgapcs(new TGAProcessor(800, 800, TGAProcessor::RGB));


const int width = 800;
const int height = 800;

vector3f eye(1, 1, 3);
vector3f center(0, 0, 0);
vector3f up(0, 1, 0);
vector3f light_dir(1, 1, 1);

float ambient;
float coeff_diff;
float coeff_spec;

class DarbouxShader :public IShader {
public:
	Matrix<2, 3, float> vary_uv;
	Matrix<3, 3, float> vary_normal;
	Matrix<3, 3, float> vary_point;
	Matrix<4, 3, float> vary_tri;
	matrix44 uniform_MIT;
	matrix44 uniform_M;

	virtual vector4f vertex(int iface, int n_tri_vertex) override {
		vary_uv.set_col(n_tri_vertex, objpcs->uv(iface, n_tri_vertex));
		//vary_normal.set_col(n_tri_vertex, objpcs->getNorm(iface, n_tri_vertex));
		vary_normal.set_col(n_tri_vertex, proj<3>(uniform_MIT*homorize<4>(objpcs->getNorm(iface, n_tri_vertex), 0.f)));
		vector4f cvertex = uniform_M * homorize<4>(objpcs->getVertex(iface, n_tri_vertex));
		vary_tri.set_col(n_tri_vertex, cvertex);
		vary_point.set_col(n_tri_vertex, proj<3>(cvertex / cvertex[3]));
		return cvertex;
	}

	virtual bool fragment(vector3f bar, TGAColor &color) override {
		vector3f normal = (vary_normal * bar).normalize();
		vector2f uv = vary_uv * bar; // for getting the value of tangent space normal

		Matrix<3, 3, float> A;
		A[0] = vary_point.col(1) - vary_point.col(0);
		A[1] = vary_point.col(2) - vary_point.col(0);
		A[2] = normal;

		Matrix<3, 3, float> AI = A.invert();
		vector3f iu = AI * vector3f(vary_uv[0][1] - vary_uv[0][0], vary_uv[0][2] - vary_uv[0][0], 0);
		vector3f iv = AI * vector3f(vary_uv[1][1] - vary_uv[1][0], vary_uv[1][2] - vary_uv[1][0], 0);

		Matrix<3, 3, float> B;
		B.set_col(0, iu.normalize());
		B.set_col(1, iv.normalize());
		B.set_col(2, normal);

		vector3f n = (B*objpcs->normal(uv)).normalize();

		float diffuse = std::max(0.f, n*light_dir);
		
		color = objpcs->diffuse(uv)*diffuse;
		return false;
	}
};

class DepthShader:public IShader {
	Matrix<3, 3, float> vary_point;

	virtual vector4f vertex(int iface, int n_tri_vertex) override {
		vector4f cvertex = transform*homorize<4>(objpcs->getVertex(iface, n_tri_vertex));
		vary_point.set_col(n_tri_vertex, proj<3>(cvertex / cvertex[3]));
		return cvertex;
	}

	virtual bool fragment(vector3f bar, TGAColor& color) {
		vector3f p = vary_point * bar;
		color = TGAColor(255, 255, 255)*(p.z / 255);
		return false;
	}
};

class GouraudShader : public IShader {
public:
	vector3f vary_intensity;
	Matrix<2,3, float> vary_uv;

	virtual vector4f vertex(int iface, int n_tri_vertex) override {
		vary_intensity[n_tri_vertex] = std::max(0.f, objpcs->getNorm(iface, n_tri_vertex)*light_dir);
		vary_uv.set_col(n_tri_vertex, objpcs->uv(iface, n_tri_vertex));
		vector4f current_vertex =  homorize<4>(objpcs->getVertex(iface, n_tri_vertex)); // use assignment?
		return transform * current_vertex;
	}
	virtual bool fragment(vector3f bar, TGAColor &color) override {
		float intensity = vary_intensity * bar;
		vector2f uv = vary_uv * bar;
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
	Matrix<4, 3, float> vary_tri;
	matrix44 uniform_M;
	matrix44 uniform_MIT;

	virtual vector4f vertex(int iface, int n_tri_vertex) override {
		vary_uv.set_col(n_tri_vertex, objpcs->uv(iface, n_tri_vertex));
		// in previous version, w in matrix has been filled with 0, in the new version, w is filled with 1
		// TODO need to debug here or in triangle function
		vector4f current_vertex = uniform_M*homorize<4>(objpcs->getVertex(iface, n_tri_vertex));
		vary_tri.set_col(n_tri_vertex, current_vertex);
		return current_vertex;
	}

	virtual bool fragment(vector3f bar, TGAColor &color) override {
		vector2f uv = vary_uv * bar;
		vector3f normal, light, reflect;
		normal = proj<3>(uniform_MIT * homorize<4>(objpcs->normal(uv))).normalize();
		light = proj<3>(uniform_M * homorize<4>(light_dir)).normalize();
		reflect = (normal * (normal*light*2.f) - light).normalize();

		float specular = pow(std::max(0.f, reflect.z), objpcs->specular(uv));
		float diffuse = std::max(0.f, normal*light);
		TGAColor temp = objpcs->diffuse(uv);
		color = temp;
		for (int i = 0; i < 3; i++) { color[i] = std::min<float>(ambient + temp[i] * (coeff_diff*diffuse + coeff_spec*specular), 255); }
		return false;
	}
};

#ifdef TEST_OBJVIEWER

int main() {
	TGAProcessor* texpcs = new TGAProcessor();
	TGAProcessor* normpcs = new TGAProcessor();
	//TGAProcessor* specpcs = new TGAProcessor();
	objpcs->loadObj();
	texpcs->read_tga_file("../obj/diablo3_pose_diffuse.tga");
	normpcs->read_tga_file("../obj/diablo3_pose_nm_tangent.tga");
	//specpcs->read_tga_file("../obj/african_head_spec.tga");
	
	objpcs->load_texture(texpcs);
	objpcs->load_normal(normpcs);
	//objpcs->load_specular(specpcs);

	ambient = 10;
	coeff_diff = 1;
	coeff_spec = 0.4;

	float* zbuffer = new float[width*height];
	for (int i = 0; i < width*height; i++) { zbuffer[i] = -std::numeric_limits<float>::max(); }

	lookat(eye, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (eye - center).norm());
	
	DarbouxShader shader;
	//shader.calcTransform();
	shader.uniform_M = Projection * ModelView;
	shader.uniform_MIT = (Projection*ModelView).invert_transpose();
	light_dir = proj<3>(shader.uniform_M*homorize<4>(light_dir, 0.f)).normalize(); // light for tangent space
	//light_dir.normalize(); // light for normal space

	for (int i = 0; i < objpcs->getFaceNumber(); i++) {
		//vector4f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			shader.vertex(i, j);
		}
		triangle(shader.vary_tri, shader, *tgapcs, zbuffer, width);
	}

	std::cout << "finished processing." << std::endl;
	tgapcs->flip_vertically();
	tgapcs->write_tga_file("../output/diablo3_pose_tangent.tga");
	return 0;
}

#endif // TEST_OBJVIEWER