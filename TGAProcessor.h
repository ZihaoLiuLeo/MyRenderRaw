#ifndef __TGAPROCESSOR_H_
#define __TGAPROCESSOR_H_

#include <fstream>

#pragma pack(push,1)
struct TGA_HEADER
{
	char idlength;
	char colormatype;
	char imagetype;
	short colormaporigin;
	short colormaplength;
	char colormapdepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char pixeldepth;
	char imagedescriptor;
};
#pragma pack(pop)

class TGAColor
{
public:
	union
	{
		struct
		{
			// be careful with the order
			unsigned char b, g, r, a;
		};
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;
	TGAColor() :val(0), bytespp(1) {};
	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A=255) :r(R), g(G), b(B), a(A), bytespp(4) {};
	TGAColor(int v, int bpp) :val(v), bytespp(bpp) {};
	TGAColor(const TGAColor &color) :val(color.val), bytespp(color.bytespp) {};
	TGAColor(const unsigned char *p, int bpp) :val(0), bytespp(bpp)
	{
		for (int i = 0; i < bpp; i++)
			raw[i] = p[i];
	}
	TGAColor& operator=(const TGAColor &color)
	{
		if (this != &color)
		{
			bytespp = color.bytespp;
			val = color.val;
		}
		return *this;
	}
	//be really careful with the order of RGB!!
	TGAColor operator*(float intensity) const {
		return TGAColor(r*intensity, g*intensity, b*intensity);
	}
	
};

class TGAProcessor
{
public:
	enum Format
	{
		GRAYSCALE = 1, RGB = 3, RGBA = 4
	};
	TGAProcessor() {};
	TGAProcessor(int w, int h, int bpp);
	TGAProcessor(const TGAProcessor &tgapcs);
	TGAProcessor& operator=(const TGAProcessor &tgapcs);
	bool read_tga_file(const char *filename);
	bool write_tga_file(const char *filename, bool rle = true);
	TGAColor get(int w, int h);
	bool set(int x, int y, TGAColor color);
	int get_width();
	int get_height();
	int get_bytespp();
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	~TGAProcessor();
	void clear();

private:
	unsigned char* data;
	int width, height, bytespp;
	bool load_rle_data(std::ifstream &in);
	bool unload_rld_data(std::ofstream &out);

};

#endif // !__TGAPROCESSOR_H_