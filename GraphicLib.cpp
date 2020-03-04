#include "GraphicLib.h"
#include <iostream>
#include <cassert>
#include <algorithm>

const TGAColor white(255, 255, 255, 255);
const TGAColor red(255, 0, 0, 255);
const TGAColor green(0, 255, 0, 255);
const TGAColor blue(0, 0, 255, 255);

GraphicLib& GraphicLib::IntBresenhamLine(Vector2i v1, Vector2i v2, TGAProcessor &image, TGAColor color)
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
	return *this;
}

//simple interpolating way
GraphicLib& GraphicLib::scanline_triangle(Vector2i t0, Vector2i t1, Vector2i t2, TGAProcessor &image, TGAColor color) {
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);
	int total_height = t2.y - t0.y;
	for (int y = t0.y; y < t1.y; y++) {
		int segment_height = t1.y - t0.y + 1;
		float alpha = (float)(y - t0.y) / total_height;
		float beta = (float)(y - t0.y) / segment_height;
		Vector2i A = t0 + (t2 - t0)*alpha;
		Vector2i B = t0 + (t1 - t0)*beta;
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
		Vector2i A = t0 + (t2 - t0)*alpha;
		Vector2i B = t1 + (t2-t1)*beta;
		/*A.y = y;
		B.y = y;
		IntBresenhamLine(A, B, image, color);*/
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; j++) {
			image.set(j, y, color);
		}
	}
	return *this;
}

// berycentric method to determine if a pixel point is inside a triangle
Vector3f barycentric_point(Vector2i *pts, Vector2i p) {
	assert(pts != nullptr);
	Vector3f u = cross(Vector3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-p[0]), 
		Vector3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-p[1]) );
	if (std::abs(u[2]) < 1) return Vector3f(-1, 1, 1);
	return Vector3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

Vector3f barycentric_point(Vector3f *pts, Vector3f p) {
	assert(pts != nullptr);
	Vector3f u = cross(Vector3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - p[0]),
		Vector3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - p[1]));
	if (std::abs(u[2]) < 1) return Vector3f(-1, 1, 1);
	return Vector3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

GraphicLib& GraphicLib::barycentric_triangle(Vector2i *pts, TGAProcessor &image, TGAColor color) {
	Vector2i bbmin, bbmax;
	bbmin.x = std::min(pts[0].x, std::min(pts[1].x, pts[2].x));
	bbmin.y = std::min(pts[0].y, std::min(pts[1].y, pts[2].y));
	bbmax.x = std::max(pts[0].x, std::max(pts[1].x, pts[2].x));
	bbmax.y = std::max(pts[0].y, std::max(pts[1].y, pts[2].y));
	Vector2i p;
	/*std::cout << "for point: " << std::endl;
	std::cout << "bound box min x is: " << bbmin.x << " bound box min y is: " << bbmin.y << std::endl;
	std::cout << "bound box max x is: " << bbmax.x << " bound box max y is: " << bbmax.y << std::endl;*/
	for (p.x = bbmin.x; p.x <= bbmax.x; p.x++) {
		for (p.y = bbmin.y; p.y <= bbmax.y; p.y++) {
			Vector3f bc_screen = barycentric_point(pts, p);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			image.set(p.x, p.y, color);
		}
	}
	return *this;
}

GraphicLib& GraphicLib::barycentric_triangle(
	Vector3f *pts,
	TGAProcessor &image, 
	TGAColor color, 
	float* zbuffer, 
	int width) {
	Vector2i bbmin, bbmax;
	bbmin.x = std::min(pts[0].x, std::min(pts[1].x, pts[2].x));
	bbmin.y = std::min(pts[0].y, std::min(pts[1].y, pts[2].y));
	bbmax.x = std::max(pts[0].x, std::max(pts[1].x, pts[2].x));
	bbmax.y = std::max(pts[0].y, std::max(pts[1].y, pts[2].y));
	Vector3f p;
	/*std::cout << "for point: " << std::endl;
	std::cout << "bound box min x is: " << bbmin.x << " bound box min y is: " << bbmin.y << std::endl;
	std::cout << "bound box max x is: " << bbmax.x << " bound box max y is: " << bbmax.y << std::endl;*/
	for (p.x = bbmin.x; p.x <= bbmax.x; p.x+=1) {
		for (p.y = bbmin.y; p.y <= bbmax.y; p.y+=1) {
			Vector3f bc_screen = barycentric_point(pts, p);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(p.x + p.y*width)] < p.z) {
				zbuffer[int(p.x + p.y*width)] = p.z;
				image.set(p.x, p.y, color);
			}
		}
	}
	return *this;
}

GraphicLib& GraphicLib::triangle_with_texture(
	Vector3f *pts,
	Vector2f *uv,
	TGAProcessor &image,
	TGAProcessor &texture,
	float intensity,
	float* zbuffer,
	int width) {
	Vector2i bbmin, bbmax;
	bbmin.x = std::min(pts[0].x, std::min(pts[1].x, pts[2].x));
	bbmin.y = std::min(pts[0].y, std::min(pts[1].y, pts[2].y));
	bbmax.x = std::max(pts[0].x, std::max(pts[1].x, pts[2].x));
	bbmax.y = std::max(pts[0].y, std::max(pts[1].y, pts[2].y));
	Vector3f p;
	/*std::cout << "for point: " << std::endl;
	std::cout << "bound box min x is: " << bbmin.x << " bound box min y is: " << bbmin.y << std::endl;
	std::cout << "bound box max x is: " << bbmax.x << " bound box max y is: " << bbmax.y << std::endl;*/
	for (p.x = bbmin.x; p.x <= bbmax.x; p.x += 1) {
		for (p.y = bbmin.y; p.y <= bbmax.y; p.y += 1) {
			Vector3f bc_screen = barycentric_point(pts, p);
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
	return *this;
}


#ifdef TEST_GL

int main()
{
	TGAProcessor pcs(200, 200, TGAProcessor::RGB);
	GraphicLib gl;
	/*gl.IntBresenhamLine(13, 80, 20, 40, pcs, white);
	gl.IntBresenhamLine(20, 40, 13, 80, pcs, red);*/
	Vector2i t0[3] = { Vector2i(10,70), Vector2i(50,160),Vector2i(70,80) };
	Vector2i t1[3] = { Vector2i(180,50), Vector2i(150,1),Vector2i(70,180) };
	Vector2i t2[3] = { Vector2i(180,150), Vector2i(120,160),Vector2i(130,180) };
	gl.barycentric_triangle(t0, pcs, red);
	gl.barycentric_triangle(t1, pcs, white);
	gl.barycentric_triangle(t2, pcs, green);
	pcs.flip_vertically();
	pcs.write_tga_file("../output/test_triangle_barycentric.tga");
	std::cout << "writing finished." << std::endl;
	return 0;
}
#endif // TEST_GL