#include "objViewer.h"
#include <vector>

const TGAColor white(255, 255, 255, 255);
Vector3f light_dir = Vector3f(1, -1, 1).normalize();

bool objViewer::drawMesh() {
	if (objpcs == nullptr) {
		std::cout << "obj not created. " << std::endl;
		return false;
	}
	if (tgapcs == nullptr) {
		std::cout << "image not created. " << std::endl;
		return false;
	}
	std::cout << "yes" << std::endl;
	std::cout << objpcs->getFaceNumber() << std::endl;
	for (int i = 0; i < objpcs->getFaceNumber(); i++) {
		
		std::vector<int> face = objpcs->face(i);
		for (int j = 0; j < 3; j++) {
			Vector3f v0 = objpcs->getVertex(face[j]);
			Vector3f v1 = objpcs->getVertex(face[(j + 1) % 3]);
			// transform to the viewport
			int x0 = (v0.x + 5.)*(tgapcs->get_width()) / 11.;
			int y0 = (v0.y + 5.)*(tgapcs->get_height()) /11.;
			int x1 = (v1.x + 5.)*(tgapcs->get_width()) / 11.;
			int y1 = (v1.y + 5.)*(tgapcs->get_height()) / 11.;
			gl.IntBresenhamLine(Vector2i(x0, y0), Vector2i(x1,y1), *tgapcs, white);
		}
	}
	tgapcs->flip_vertically();
	tgapcs->write_tga_file(output_.c_str());
	std::cout << "Finished Writing." << std::endl;
	return true;
}

Vector3f objViewer::world2screen(Vector3f v) {
	return Vector3f(int((v.x+1.)*width/2.+.5),int((v.y+1.)*height/2.+.5),v.z);
}

bool objViewer::triangle_draw() {
	if (objpcs == nullptr) {
		std::cout << "obj not created. " << std::endl;
		return false;
	}
	if (tgapcs == nullptr) {
		std::cout << "image not created. " << std::endl;
		return false;
	}

	float* zbuffer = new float[width*height];
	for (int i = 0; i < width*height; i++) { zbuffer[i] = INT_MIN; }

	Vector3f light_dir(0, 0, -1);
	for (int i = 0; i < objpcs->getFaceNumber(); i++) {

		std::vector<int> face = objpcs->face(i);
		
		Vector3f screen_coords[3];
		Vector3f world_coords[3];
		
		for (int j = 0; j < 3; j++) {
			world_coords[j] = objpcs->getVertex(face[j]);
			
			// transform to the viewport
			screen_coords[j] = world2screen(world_coords[j]);
		}
		Vector3f norm = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		norm.normalize();
		float intensity = norm * light_dir;
		if (intensity > 0) {
			gl.barycentric_triangle(screen_coords, *tgapcs, TGAColor(255*intensity, 255 * intensity, 255 * intensity, 255),zbuffer,width);
		}
	}
	tgapcs->flip_vertically();
	tgapcs->write_tga_file(output_.c_str());
	std::cout << "Finished Writing." << std::endl;
	return true;
}

//bool objViewer::triangle_zbuffer_draw() {
//	if (objpcs == nullptr) {
//		std::cout << "obj not created. " << std::endl;
//		return false;
//	}
//	if (tgapcs == nullptr) {
//		std::cout << "image not created. " << std::endl;
//		return false;
//	}
//	if (tgapcs_texture == nullptr) {
//		std::cout << "texture not created. " << std::endl;
//		return false;
//	}
//
//	float* zbuffer = new float[width*height];
//	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
//
//	Vector3f light_dir(0, 0, -1);
//	for (int i = 0; i < objpcs->getFaceNumber(); i++) {
//
//		std::vector<int> face = objpcs->face(i);
//		std::vector<int> uv = objpcs->uv(i);
//		Vector3f screen_coords[3];
//		Vector3f world_coords[3];
//		Vector2f uvmap[3];
//		for (int j = 0; j < 3; j++) {
//			world_coords[j] = objpcs->getVertex(face[j]);
//			Vector3f uv_coords = objpcs->getTex(uv[j]);
//			//calculate texture coordinates
//			float x = uv_coords.x * tgapcs_texture->get_width();
//			float y = uv_coords.y * tgapcs_texture->get_height();
//			uvmap[j] = Vector2f(x, y);
//			// transform to the viewport
//			screen_coords[j] = world2screen(world_coords[j]);
//		}
//		//std::cout << uvmap[0]  << "/" << uvmap[1] << "/" << uvmap[2] << std::endl;
//		//set light intensity
//		Vector3f norm = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
//		norm.normalize();
//		float intensity = norm * light_dir;
//		//render
//		if (intensity > 0) {
//			//std::cout << intensity << std::endl;
//			gl.triangle_with_texture(
//				screen_coords, 
//				uvmap, 
//				*tgapcs, 
//				*tgapcs_texture, 
//				intensity,
//				zbuffer, 
//				width);
//		}
//	}
//	tgapcs->flip_vertically();
//	tgapcs->write_tga_file(output_.c_str());
//	std::cout << "Finished Writing." << std::endl;
//	return true;
//}

Matrix viewport(int x, int y, int w, int h, int depth=255) {
	Matrix m = Matrix::identity(4);
	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;

	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;
	return m;
}

Matrix lookat(Vector3f eye, Vector3f center, Vector3f up) {
	Vector3f z = (eye - center).normalize();
	Vector3f x = (up^z).normalize();
	Vector3f y = (z^x).normalize();
	// TODO need debug
	Matrix modelView = Matrix::identity(4);
	
	for (int i = 0; i < 3; i++) {
		modelView[0][i] = x[i];
		modelView[1][i] = y[i];
		modelView[2][i] = z[i];
		modelView[i][3] = -center[i];
	}
	return modelView;
}

bool objViewer::triangle_lookat_draw() {
	Vector3f eye(0, 3, 3);
	Vector3f center(0, 0, 0);
	
	if (objpcs == nullptr) {
		std::cout << "obj not created. " << std::endl;
		return false;
	}
	if (tgapcs == nullptr) {
		std::cout << "image not created. " << std::endl;
		return false;
	}

	int* zbuffer = new int[width*height];
	for (int i = 0; i < width*height; i++) { zbuffer[i] = INT_MIN; }

	{ // draw
		Matrix ModelView = lookat(eye, center, Vector3f(0, 1, 0));
		Matrix Projection = Matrix::identity(4);
		Matrix ViewPort = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
		Projection[3][2] = -1.f / (eye - center).norm();

		Matrix z = (ViewPort*Projection*ModelView);
		
		for (int i = 0; i < objpcs->getFaceNumber(); i++) {
			std::vector<int> face = objpcs->face(i);
			Vector3f screen_coords[3];
			float intensity[3];
			for (int j = 0; j < 3; j++) {
				Vector3f v = objpcs->getVertex(face[j]);
				screen_coords[j] = Vector3f(z*Matrix(v));
				intensity[j] = objpcs->getNorm(i,j)*light_dir;
			}
			gl.triangle_gouroud(
				screen_coords[0],
				screen_coords[1],
				screen_coords[2],
				intensity[0],
				intensity[1],
				intensity[2],
				*tgapcs,
				zbuffer,
				width,
				height
			);
		}
	}
	tgapcs->flip_vertically();
	tgapcs->write_tga_file(output_.c_str());
	std::cout << "Finished Writing." << std::endl;
	return true;
}

#ifdef TEST_OBJVIEWER

int main() {
	Optr objpcs(new objPcs("../obj/african_head.obj"));
	objpcs->loadObj();
	Tptr tgapcs(new TGAProcessor(800, 800, TGAProcessor::RGB));
	Tptr tgapcs_texture(new TGAProcessor);
	//tgapcs_texture->read_tga_file("../obj/african_head_diffuse.tga");
	//tgapcs_texture->flip_vertically();
	//std::cout << tgapcs_texture->get_width() << "/" << tgapcs_texture->get_height() << std::endl;

	objViewer objviewer(tgapcs, objpcs);
	objviewer.output("../output/african_head_gouroud1.tga");
	
	objviewer.triangle_lookat_draw();	
}

#endif // TEST_OBJVIEWER