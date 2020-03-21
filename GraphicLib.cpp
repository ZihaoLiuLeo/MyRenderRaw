#include "GraphicLib.h"
#include <iostream>
#include <cassert>
#include <algorithm>

matrix44 ModelView;
matrix44 ViewPort;
matrix44 Projection;

void IntBresenhamLine(vector2i v1, vector2i v2, TGAProcessor &image, TGAColor color)
{
	bool steep = false;
	if (std::abs(v1.x - v2.x)<std::abs(v1.y - v2.y)) { // if the line is steep, we transpose the image 
		std::swap(v1.x, v1.y);
		std::swap(v2.x, v2.y);
		steep = true;
	}
	if (v1.x>v2.x) { // make it left−to−right 
		std::swap(v2.x, v1.x);
		std::swap(v2.y, v1.y);
	}
	int x, y, dx, dy, e;
	dx = v2.x - v1.x, dy = std::abs(v2.y - v1.y), e = -dx;
	x = v1.x, y = v1.y;
	for (int i = 0; i <= dx; i++)
	{
		if (steep)image.set(y, x, color);
		else image.set(x, y, color);
		x++;
		e += 2 * dy;
		if (e >= 0) { y+=(v2.y>v1.y?1:-1); e -= 2 * dx;}
	}
}

//simple interpolating way
void scanline_triangle(vector2i t0, vector2i t1, vector2i t2, TGAProcessor &image, TGAColor color) {
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);
	int total_height = t2.y - t0.y;
	for (int y = t0.y; y < t1.y; y++) {
		int segment_height = t1.y - t0.y + 1;
		float alpha = (float)(y - t0.y) / total_height;
		float beta = (float)(y - t0.y) / segment_height;
		vector2i A = t0 + (t2 - t0)*alpha;
		vector2i B = t0 + (t1 - t0)*beta;
		/*A.y = y;
		B.y = y;
		IntBresenhamLine(A, B, image, color);*/
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++) {
			image.set(j, y, color);
		}
	}
	for (int y = t1.y; y <= t2.y; y++) {
		int segment_height = t2.y - t1.y + 1;
		float alpha = (float)(y - t0.y) / total_height;
		float beta = (float)(y - t1.y) / segment_height;
		vector2i A = t0 + (t2 - t0)*alpha;
		vector2i B = t1 + (t2-t1)*beta;
		/*A.y = y;
		B.y = y;
		IntBresenhamLine(A, B, image, color);*/
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++) {
			image.set(j, y, color);
		}
	}
}

// berycentric method to determine if a pixel point is inside a triangle
vector3f barycentric_point(vector2f *pts, vector2f p) {
	assert(pts != nullptr);
	vector3f u = cross(vector3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-p[0]), 
		vector3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-p[1]) );
	if (std::abs(u[2]) < 1e-2) return vector3f(-1, 1, 1);
	return vector3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

vector3f barycentric_point(vector3f *pts, vector3f p) {
	assert(pts != nullptr);
	vector3f u = cross(vector3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - p[0]),
		vector3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - p[1]));
	if (std::abs(u[2]) < 1) return vector3f(-1, 1, 1);
	return vector3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

//void barycentric_triangle(vector2i *pts, TGAProcessor &image, TGAColor color) {
//	vector2i bbmin, bbmax;
//	bbmin.x = std::min(pts[0].x, std::min(pts[1].x, pts[2].x));
//	bbmin.y = std::min(pts[0].y, std::min(pts[1].y, pts[2].y));
//	bbmax.x = std::max(pts[0].x, std::max(pts[1].x, pts[2].x));
//	bbmax.y = std::max(pts[0].y, std::max(pts[1].y, pts[2].y));
//	vector2i p;
//	/*std::cout << "for point: " << std::endl;
//	std::cout << "bound box min x is: " << bbmin.x << " bound box min y is: " << bbmin.y << std::endl;
//	std::cout << "bound box max x is: " << bbmax.x << " bound box max y is: " << bbmax.y << std::endl;*/
//	for (p.x = bbmin.x; p.x <= bbmax.x; p.x++) {
//		for (p.y = bbmin.y; p.y <= bbmax.y; p.y++) {
//			vector3f bc_screen = barycentric_point(pts, p);
//			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
//			image.set(p.x, p.y, color);
//		}
//	}
//}

void barycentric_triangle(
	vector3f *pts,
	TGAProcessor &image, 
	TGAColor color, 
	float* zbuffer, 
	int width) {
	vector2i bbmin, bbmax;
	bbmin.x = std::min(pts[0].x, std::min(pts[1].x, pts[2].x));
	bbmin.y = std::min(pts[0].y, std::min(pts[1].y, pts[2].y));
	bbmax.x = std::max(pts[0].x, std::max(pts[1].x, pts[2].x));
	bbmax.y = std::max(pts[0].y, std::max(pts[1].y, pts[2].y));
	vector3f p;
	/*std::cout << "for point: " << std::endl;
	std::cout << "bound box min x is: " << bbmin.x << " bound box min y is: " << bbmin.y << std::endl;
	std::cout << "bound box max x is: " << bbmax.x << " bound box max y is: " << bbmax.y << std::endl;*/
	for (p.x = bbmin.x; p.x <= bbmax.x; p.x+=1) {
		for (p.y = bbmin.y; p.y <= bbmax.y; p.y+=1) {
			vector3f bc_screen = barycentric_point(pts, p);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(p.x + p.y*width)] < p.z) {
				zbuffer[int(p.x + p.y*width)] = p.z;
				image.set(p.x, p.y, color);
			}
		}
	}
}

// flat shading with triangle facets
void triangle_with_texture(
	vector3f *pts,
	vector2f *uv,
	TGAProcessor &image,
	TGAProcessor &texture,
	float intensity,
	float* zbuffer,
	int width) {
	vector2i bbmin, bbmax;
	bbmin.x = std::min(pts[0].x, std::min(pts[1].x, pts[2].x));
	bbmin.y = std::min(pts[0].y, std::min(pts[1].y, pts[2].y));
	bbmax.x = std::max(pts[0].x, std::max(pts[1].x, pts[2].x));
	bbmax.y = std::max(pts[0].y, std::max(pts[1].y, pts[2].y));
	vector3f p;
	/*std::cout << "for point: " << std::endl;
	std::cout << "bound box min x is: " << bbmin.x << " bound box min y is: " << bbmin.y << std::endl;
	std::cout << "bound box max x is: " << bbmax.x << " bound box max y is: " << bbmax.y << std::endl;*/
	for (p.x = bbmin.x; p.x <= bbmax.x; p.x += 1) {
		for (p.y = bbmin.y; p.y <= bbmax.y; p.y += 1) {
			vector3f bc_screen = barycentric_point(pts, p);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			p.z = 0;
			int u = 0;
			int v = 0;
			for (int i = 0; i < 3; i++) {
				p.z += pts[i][2] * bc_screen[i];
				u += (int)(uv[i][0] * bc_screen[i]);
				v += (int)(uv[i][1] * bc_screen[i]);
			}

			if (zbuffer[int(p.x + p.y*width)] < p.z) {
				zbuffer[int(p.x + p.y*width)] = p.z;
				TGAColor color = texture.get(u, v);
				image.set(p.x, p.y, color*intensity);
			}
		}
	}
}

//scanline interpolation
void triangle_gouroud(
	vector3i *t, 
	float *intensity_y,
	TGAProcessor &image, 
	int *zbuffer, 
	int width, 
	int height) {
	if (t[0].y == t[1].y&& t[0].y == t[2].y)return;
	if (t[0].y > t[1].y) { std::swap(t[0], t[1]); std::swap(intensity_y[0], intensity_y[1]); }
	if (t[0].y > t[2].y) { std::swap(t[0], t[2]); std::swap(intensity_y[0], intensity_y[2]); }
	if (t[1].y > t[2].y) { std::swap(t[1], t[2]); std::swap(intensity_y[1], intensity_y[2]); }

	int total_height = t[2].y - t[0].y;
	for (int i = 0; i < total_height; i++) {
		bool second_half = i > t[1].y - t[0].y || t[1].y == t[0].y; // need to debug
		int segment_height = second_half ? t[2].y - t[1].y : t[1].y - t[0].y;
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t[1].y - t[0].y : 0)) / segment_height;
		vector3i A = t[0] + vector3i(vector3f(t[2] - t[0])*alpha);
		vector3i B = second_half ? t[1] + vector3i(vector3f(t[2] - t[1])*beta) : t[0] + vector3i(vector3f(t[1] - t[0])*beta); // buggy place, beta alpha switch
		float ityA = intensity_y[0] + (intensity_y[2] - intensity_y[0])*alpha;
		float ityB = second_half ? intensity_y[1] + (intensity_y[2] - intensity_y[1])*beta : intensity_y[0] + (intensity_y[1] - intensity_y[0])*beta;
		if (A.x > B.x) { std::swap(A, B); std::swap(ityA, ityB); }
		for (int j = A.x; j <= B.x; j++) {
			float phi = B.x == A.x ? 1. : (float)(j - A.x) / (B.x - A.x);
			vector3i p = vector3f(A) + vector3f(B - A)*phi;
			float ityP = ityA + (ityB - ityA)*phi;
			int idx = p.x + p.y*width;
			if (p.x >= width || p.y >= height || p.y < 0 || p.x < 0) continue;
			if (zbuffer[idx] < p.z) {
				zbuffer[idx] = p.z;
				image.set(p.x, p.y, TGAColor(255, 255, 255)*ityP);
			}
		}
	}
}

void triangle(
	vector4f* t,
	IShader& shader,
	TGAProcessor& image,
	int* zbuffer,
	int width) {
	vector2i bbmin, bbmax;
	bbmin.x = std::min(t[0][0] / t[0][3], std::min(t[1][0] / t[1][3], t[2][0] / t[2][3]));
	bbmin.y = std::min(t[0][1] / t[0][3], std::min(t[1][1] / t[1][3], t[2][1] / t[2][3]));
	bbmax.x = std::max(t[0][0] / t[0][3], std::max(t[1][0] / t[1][3], t[2][0] / t[2][3]));
	bbmax.y = std::max(t[0][1] / t[0][3], std::max(t[1][1] / t[1][3], t[2][1] / t[2][3]));
	vector2i p;
	TGAColor color;
	for (p.x = bbmin.x; p.x <= bbmax.x; p.x = p.x + 1) {
		for (p.y = bbmin.y; p.y <= bbmax.y;  p.y++) {
			vector2f pts[3];
			for (int i = 0; i < 3; i++) { pts[i] = proj<2>(t[i] / t[i][3]); }
			vector3f c = barycentric_point(pts, p);
			//  z/w need to be used to calculate the zbuffer depth
			float z = t[0][2] * c.x + t[1][2] * c.y + t[2][2] * c.z;
			float w = t[0][3] * c.x + t[1][3] * c.y + t[2][3] * c.z;
			int depth = std::max(0, std::min(255, int(z / w + .5)));
			if (c.x < 0 || c.y < 0 || c.z < 0 || zbuffer[p.x+p.y*width]>depth) continue;
			bool discard = shader.fragment(c, color);
			if (!discard) {
				zbuffer[p.x + p.y*width] = depth;
				image.set(p.x, p.y, color);
			}
			/*p.z = 0;
			for (int i = 0; i < 3; i++)p.z += t[i][2] * c[i];
			if (zbuffer[p.x + p.y*width] < p.z) {
				zbuffer[p.x + p.y*width] = p.z;
				std::cout << p.z << std::endl;
				bool discard = shader.fragment(c, color);
				if (!discard) {
					image.set(p.x, p.y, color);
				}
			}*/
		}
	}
}

void lookat(vector3f eye, vector3f center, vector3f up) {
	vector3f z = (eye - center).normalize();
	vector3f x = cross(up,z).normalize();
	vector3f y = cross(z,x).normalize();
	
	ModelView = identity<matrix44,4>();

	for (int i = 0; i < 3; i++) {
		ModelView[0][i] = x[i];
		ModelView[1][i] = y[i];
		ModelView[2][i] = z[i];
		ModelView[i][3] = -center[i];
	}
}

void viewport(int x, int y, int w, int h, int depth) {
	ViewPort = identity<matrix44, 4>();
	ViewPort[0][3] = x + w / 2.f;
	ViewPort[1][3] = y + h / 2.f;
	ViewPort[2][3] = depth / 2.f;

	ViewPort[0][0] = w / 2.f;
	ViewPort[1][1] = h / 2.f;
	ViewPort[2][2] = depth / 2.f;
}

void projection(float coeff) {
	Projection = identity<matrix44, 4>();
	Projection[3][2] = coeff;
}

void IShader::calcTransform() {
	transform = ViewPort * Projection * ModelView;
}

#ifdef TEST_GL

int main()
{
	TGAProcessor pcs(200, 200, TGAProcessor::RGB);
	GraphicLib gl;
	/*gl.IntBresenhamLine(13, 80, 20, 40, pcs, white);
	gl.IntBresenhamLine(20, 40, 13, 80, pcs, red);*/
	Vector2i t[0][3] = { Vector2i(10,70), Vector2i(50,160),Vector2i(70,80) };
	Vector2i t1[3] = { Vector2i(180,50), Vector2i(150,1),Vector2i(70,180) };
	Vector2i t2[3] = { Vector2i(180,150), Vector2i(120,160),Vector2i(130,180) };
	gl.barycentric_triangle(t[0], pcs, red);
	gl.barycentric_triangle(t1, pcs, white);
	gl.barycentric_triangle(t2, pcs, green);
	pcs.flip_vertically();
	pcs.write_tga_file("../output/test_triangle_barycentric.tga");
	std::cout << "writing finished." << std::endl;
	return 0;
}
#endif // TEST_GL