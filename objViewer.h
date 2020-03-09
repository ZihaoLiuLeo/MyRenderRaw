#ifndef __OBJVIEWER_H_
#define __OBJ_VIEWER_H_
#include "../ImageProcessor/TGAProcessor.h"
#include "../objProcessor/objProcessor.h"
#include "../GraphicLib/GraphicLib.h"
#include <memory>

using Optr = std::shared_ptr<objPcs>;
using Tptr = std::shared_ptr<TGAProcessor>;
class objViewer {
public:
	objViewer() = default;
	objViewer(Tptr tgaPcs, Optr objPcs):tgapcs(tgaPcs),objpcs(objPcs) {}
	objViewer(Tptr tgaPcs, Tptr tgaPcs_texture, Optr objPcs):
		tgapcs(tgaPcs), 
		tgapcs_texture(tgaPcs_texture),
		objpcs(objPcs) {}
	bool drawMesh();
	bool triangle_draw();
	//bool triangle_zbuffer_draw();
	bool triangle_lookat_draw();
	Vector3f world2screen(Vector3f v);

	std::string& output() { return output_; }
	std::string output() const { return output_; }
	void output(const std::string& output) { output_ = output; }

private:
	Tptr tgapcs;
	Tptr tgapcs_texture;
	Optr objpcs;
	//TGAProcessor *tgapcs;
	//objPcs *objpcs;
	GraphicLib gl;
	std::string output_;
	int width = tgapcs->get_width();
	int height = tgapcs->get_height();
};


#endif // !__OBJVIEWER_H_
