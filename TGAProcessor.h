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
	unsigned char bgra[4];
	unsigned char bytespp;

	TGAColor() : bgra(), bytespp(1) {
		for (int i = 0; i < 4; i++) bgra[i] = 0;
	}

	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A=255) :bgra(), bytespp(4) {
		bgra[0] = B;
		bgra[1] = G;
		bgra[2] = R;
		bgra[3] = A;
	}

	TGAColor(unsigned char v) :bgra(), bytespp(1) {
		for (int i = 0; i < 4; i++)bgra[i] = 0;
		bgra[0] = v;
	}
	
	TGAColor(const unsigned char *p, unsigned char bpp) :bgra(), bytespp(bpp) {
		for (int i = 0; i < (int)bpp; i++) {
			bgra[i] = p[i];
		}
		for (int i = bpp; i < 4; i++) {
			bgra[i] = 0;
		}
	}

	// lighting bug fixed...
	// error on intensity greater than  1 and less than 0
	TGAColor operator*(float intensity) const {
		TGAColor res = *this;
		intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
		for (int i = 0; i < 4; i++)res.bgra[i] = bgra[i] * intensity;
		return res;
	}
	
	unsigned char &operator[](const int i){
		return bgra[i];
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