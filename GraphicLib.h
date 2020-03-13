#ifndef __GRAPHICLIB_H_
#define __GRAPHICLIB_H_
#include "../ImageProcessor/TGAProcessor.h"
#include "../Geometry/geometry.h"
#include <vector>

extern Matrix ModelView;
extern Matrix ViewPort;
extern Matrix Projection;

class IShader
{
public:
	virtual ~IShader() = default;
	virtual Matrix vertex(int iface, int nvertex) = 0;
	virtual bool fragment(Vector3f bar, TGAColor &color) = 0;
	virtual void calcTransform();

	Matrix transform;
};

void IntBresenhamLine(Vector2i v1, Vector2i v2, TGAProcessor &image, TGAColor color);
void scanline_triangle(Vector2i t0, Vector2i t1, Vector2i t2, TGAProcessor &image, TGAColor color);
void barycentric_triangle(Vector2i *pts, TGAProcessor &image, TGAColor color);
void barycentric_triangle(Vector3f *pts, TGAProcessor &image, TGAColor color, float* zbuffer, int width);
void triangle_with_texture(Vector3f *pts, Vector2f *uv, TGAProcessor &image, TGAProcessor &texture, float intensity, float* zbuffer, int width);
void triangle_gouroud(Vector3i *t, float *intensity_y, TGAProcessor &image, int *zbuffer, int width, int height);
void triangle(Vector3f *t, IShader& shader, TGAProcessor& image, int* zbuffer, int width);

void lookat(Vector3f eye, Vector3f center, Vector3f up);
void projection(float coeff = 0.f);
void viewport(int x, int y, int w, int h, int depth=255);

#endif // !__GRAPHICLIB_H_
