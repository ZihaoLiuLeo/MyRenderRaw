#ifndef __GRAPHICLIB_H_
#define __GRAPHICLIB_H_
#include "../ImageProcessor/TGAProcessor.h"
#include "../Geometry/geometry.h"
#include <vector>

class GraphicLib
{
public:
	GraphicLib() = default;

	GraphicLib& IntBresenhamLine(Vector2i v1, Vector2i v2,TGAProcessor &image, TGAColor color);
	GraphicLib& scanline_triangle(Vector2i t0, Vector2i t1, Vector2i t2, TGAProcessor &image, TGAColor color);
	GraphicLib& barycentric_triangle(Vector2i *pts, TGAProcessor &image, TGAColor color);
	GraphicLib& barycentric_triangle(Vector3f *pts, TGAProcessor &image, TGAColor color, float* zbuffer, int width);
	GraphicLib& triangle_with_texture(Vector3f *pts, Vector2f *uv, TGAProcessor &image, TGAProcessor &texture, float intensity, float* zbuffer, int width);

private:
};
#endif // !__GRAPHICLIB_H_
