#ifndef __GRAPHICLIB_H_
#define __GRAPHICLIB_H_
#include "../ImageProcessor/TGAProcessor.h"
#include "../Geometry2/transform.h"
#include <vector>

extern matrix44 ModelView;
extern matrix44 ViewPort;
extern matrix44 Projection;

class IShader
{
public:
	virtual ~IShader() = default;
	virtual vector4f vertex(int iface, int nvertex) = 0;
	virtual bool fragment(vector3f bar, TGAColor &color) = 0;
	virtual void calcTransform();

	matrix44 transform;
};

void IntBresenhamLine(vector2i v1, vector2i v2, TGAProcessor &image, TGAColor color);
void scanline_triangle(vector2i t0, vector2i t1, vector2i t2, TGAProcessor &image, TGAColor color);
//void barycentric_triangle(vector2i *pts, TGAProcessor &image, TGAColor color);
void barycentric_triangle(vector3f *pts, TGAProcessor &image, TGAColor color, float* zbuffer, int width);
void triangle_with_texture(vector3f *pts, vector2f *uv, TGAProcessor &image, TGAProcessor &texture, float intensity, float* zbuffer, int width);
void triangle_gouroud(vector3i *t, float *intensity_y, TGAProcessor &image, int *zbuffer, int width, int height);
void triangle(Matrix<4,3,float> &t, IShader& shader, TGAProcessor& image, float* zbuffer, int width);

void lookat(vector3f eye, vector3f center, vector3f up);
void projection(float coeff = 0.f);
void viewport(int x, int y, int w, int h, float depth=255.f);

#endif // !__GRAPHICLIB_H_
