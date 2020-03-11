#include <iostream>
#include "TGAProcessor.h"
TGAProcessor::TGAProcessor(int w, int h, int bpp) : data(nullptr), width(w), height(h), bytespp(bpp)
{
	unsigned long nbytes = width * height * bytespp;
	data = new unsigned char[nbytes];
	std::memset(data, 0, nbytes);
}

TGAProcessor::TGAProcessor(const TGAProcessor &tgapcs) : data(nullptr), width(tgapcs.width), height(tgapcs.height), bytespp(tgapcs.bytespp)
{
	unsigned long nbytes = width * height * bytespp;
	data = new unsigned char[nbytes];
	std::memcpy(data, tgapcs.data, nbytes);
}

TGAProcessor::~TGAProcessor()
{
	if (data) delete[] data;
}

TGAProcessor& TGAProcessor::operator=(const TGAProcessor &tgapcs)
{
	if (this != &tgapcs)
	{
		if (data)delete[] data;
		width = tgapcs.width;
		height = tgapcs.height;
		bytespp = tgapcs.bytespp;
		unsigned long nbytes = width * height*bytespp;
		data = new unsigned char[nbytes];
		std::memcpy(data, tgapcs.data, nbytes);
	}
	return *this;
}

bool TGAProcessor::read_tga_file(const char *filename)
{
	if (data) delete[] data;
	data = nullptr;
	std::ifstream in;
	in.open(filename, std::ios::binary);
	if (!in.is_open())
	{
		in.close();
		std::cerr << "cannot open file " << filename << std::endl;
		return false;
	}
	TGA_HEADER header;
	in.read((char*)&header, sizeof(header));
	if (!in.good())
	{
		in.close();
		std::cerr << "an error occrued while reading the header " << std::endl;
		return false;
	}
	width = header.width;
	height = header.height;
	bytespp = header.pixeldepth >> 3;
	if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA))
	{
		in.close();
		std::cerr << "bad bpp (or width/height) value." << std::endl;
		return false;
	}
	unsigned long nbytes = width * height*bytespp;
	data = new unsigned char[nbytes];
	//image type 3: black and white(unmapped image)
	//imgae type 2: true color image
	//no RLE
	if (header.imagetype == 3 || header.imagetype == 2)
	{
		in.read((char*)data, nbytes);
		if (!in.good())
		{
			in.close();
			std::cerr << "an error occured while reading the data" << std::endl;
			return false;
		}
	}
	//RLE
	else if (header.imagetype == 10 || header.imagetype == 11)
	{
		if (!load_rle_data(in))
		{
			in.close();
			std::cerr << "an error occured while reading the data." << std::endl;
			return false;
		}
	}
	else
	{
		in.close();
		std::cerr << "unknown file format " << (int)header.imagetype << std::endl;
		return false;
	}

	if (!(header.imagedescriptor & 0x20))
	{
		flip_vertically();
	}
	if (!(header.imagedescriptor & 0x10))
	{
		flip_horizontally();
	}
	in.close();
	return true;
}

bool TGAProcessor::load_rle_data(std::ifstream & in)
{
	unsigned long pixelcount = width * height;
	unsigned long currentpixel = 0;
	unsigned long currentbyte = 0;
	TGAColor colorbuffer;
	do
	{
		unsigned char chunkheader = 0;
		chunkheader = in.get();
		if (!in.good())
		{
			return false;
		}
		if (chunkheader < 128)
		{
			for (int i = 0; i < chunkheader+1; i++)
			{
				in.read((char*)colorbuffer.bgra, bytespp);
				if (!in.good())
				{
					return false;
				}
				for (int t = 0; t < bytespp; t++)
				{
					data[currentbyte++] = colorbuffer.bgra[t];
				}
				currentpixel++;
				if (currentpixel > pixelcount)
				{
					return false;
				}
			}
		}
		//pixel count longer than 128 bytes, using multiple packets.
		else
		{
			chunkheader -= 127;
			in.read((char*)colorbuffer.bgra, bytespp);
			if (!in.good())
			{
				return false;
			}
			for (int i = 0; i < chunkheader; i++)
			{
				for (int t = 0; t < bytespp; t++)
					data[currentbyte++] = colorbuffer.bgra[t];
				currentpixel++;
				if (currentpixel > pixelcount)
				{
					std::cerr << "Too many pixels read;" << std::endl;
					return false;
				}
			}
		}
	} while (currentpixel < pixelcount);
	return true;
}

bool TGAProcessor::write_tga_file(const char *filename, bool rle)
{
	unsigned char developer_area_ref[4] = { 0, 0, 0, 0 };
	unsigned char extension_area_ref[4] = { 0, 0, 0, 0 };
	unsigned char footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };
	std::ofstream out;
	out.open(filename, std::ios::binary);
	if (!out.is_open())
	{
		std::cerr << "can't open file " << filename << std::endl;
		out.close();
		return false;
	}
	TGA_HEADER header;
	std::memset((void*)&header, 0, sizeof(header));
	header.pixeldepth = bytespp << 3;
	header.width = width;
	header.height = height;
	header.imagetype = (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
	header.imagedescriptor = 0x20;
	out.write((char*)&header, sizeof(header));
	if (!out.good())
	{
		std::cout << "can't dum the tga file" << std::endl;
		out.close();
		return false;
	}
	if (!rle)
	{
		out.write((char*)data, width*height*bytespp);
		if (!out.good())
		{
			std::cerr << "can't unload raw data" << std::endl;
			out.close();
			return false;
		}
	}
	else
	{
		if (!unload_rld_data(out))
		{
			out.close();
			std::cerr << "can't unload raw data" << std::endl;
			return false;
		}
	}
	out.write((char*)developer_area_ref, sizeof(developer_area_ref));
	if (!out.good())
	{
		std::cerr << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	out.write((char*)extension_area_ref, sizeof(extension_area_ref));
	if (!out.good())
	{
		std::cerr << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	out.write((char*)footer, sizeof(footer));
	if (!out.good())
	{
		std::cerr << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	out.close();
	return true;
}

bool TGAProcessor::unload_rld_data(std::ofstream &out)
{
	const unsigned char max_chunk_length = 128;
	unsigned long npixels = width * height;
	unsigned long curpix = 0;
	while (curpix < npixels)
	{
		unsigned long chunkstart = curpix * bytespp;
		unsigned long curbyte = curpix * bytespp;
		unsigned char run_length = 1;
		bool raw = true;
		while (curpix + run_length < npixels && run_length < max_chunk_length)
		{
			bool eq_next_bit = true;
			// if the color is the same as the next pixel
			for (int t = 0; eq_next_bit&&t < bytespp; t++)
			{
				eq_next_bit = (data[curbyte + t] == data[curbyte + t + bytespp]);
			}
			curbyte += bytespp;
			if (run_length == 1)
			{
				raw =! eq_next_bit;
			}
			if (raw && eq_next_bit)
			{
				run_length++;
				break;
			}
			if (!raw & !eq_next_bit)
			{
				break;
			}
			run_length++;
		}
		curpix += run_length;
		out.put(raw ? run_length - 1 : run_length + 127);
		if (!out.good())
		{
			std::cerr << "cant dump the tga file" << std::endl;
			return false;
		}
		out.write((char*)(data + chunkstart), (raw ? run_length * bytespp : bytespp));
		if (!out.good())
		{
			std::cerr << "can't dump the tga file" << std::endl;
			return false;
		}
	}
	return true;
}

TGAColor TGAProcessor::get(int x, int y)
{
	if (!data || x < 0 || y < 0 || x >= width || y >= width)
	{
		return TGAColor();
	}
	return TGAColor(data + (x + y * width)*bytespp, bytespp);
}

bool TGAProcessor::set(int x, int y, TGAColor color)
{
	if (!data || x < 0 || y < 0 || x >= width || y >= width)
	{
		return false;
	}
	std::memcpy(data + (x + y * width)*bytespp, color.bgra, bytespp);
	return false;
}

int TGAProcessor::get_width()
{
	return width;
}

int TGAProcessor::get_height()
{
	return height;
}

int TGAProcessor::get_bytespp()
{
	return bytespp;
}

bool TGAProcessor::flip_horizontally()
{
	if (!data) return false;
	int half = width >> 1;
	for (int i = 0; i < half; i++)
	{
		for (int j = 0; j < height; j++)
		{
			TGAColor color1 = get(i, j);
			TGAColor color2 = get(width - 1 - i, j);
			set(i, j, color2);
			set(width - 1 - i, j, color1);
		}
	}
	return true;
}

bool TGAProcessor::flip_vertically()
{
	if (!data) return false;
	int half = height >> 1;
	unsigned long bytespl = width * bytespp;
	unsigned char *line = new unsigned char[bytespl];
	for (int i = 0; i < half; i++)
	{
		unsigned long l1_start = i * bytespl;
		unsigned long l2_start = (height - 1 - i)*bytespl;
		std::memmove((void*)line, (void*)(data+l1_start), bytespl);
		std::memmove((void*)(data+l1_start), (void*)(data+l2_start), bytespl);
		std::memmove((void*)(data+l2_start), (void*)line, bytespl);
	}
	delete[] line;
	return true;
}

bool TGAProcessor::scale(int w,int h)
{
	if (w <= 0 || h <= 0 || !data) return false;
	unsigned char *tdata = new unsigned char[w*h*bytespp];
	int nscanline = 0;
	int oscanline = 0;
	int delta_y = 0;
	unsigned long nlinebytes = w * bytespp;
	unsigned long olinebytes = width * bytespp;
	for (int j = 0; j < height; j++)
	{
		int delta_x = width - w;
		int nx = -bytespp;
		int ox = -bytespp;
		for (int i = 0; i < width; i++)
		{
			ox += bytespp;
			delta_x += w;
			//this step is equal to scan the i bit by w/width times
			//and fill it to tdata+nscanline
			while (delta_x >= (int)width)
			{
				delta_x -= width;
				nx += bytespp;
				std::memcpy(tdata + nscanline + nx, data + oscanline + ox, bytespp);
			}
		}
		delta_y += h;
		oscanline += olinebytes;
		while (delta_y >= (int)height)
		{
			//since the last x line has been scanned, so we will skip one scan line
			if (delta_y >= (int)height << 1)
				std::memcpy(tdata + nscanline + nlinebytes, data + oscanline + ox, bytespp);
			delta_y -= height;
			nscanline += nlinebytes;
		}
	}
	delete[] data;
	data = tdata;
	width = w;
	height = h;
	return true;
}

void TGAProcessor::clear()
{
	std::memset((void*)data, 0, width * height * bytespp);
}


#ifdef TEST_TGA

int main()
{
	const TGAColor white(255, 255, 255, 255);
	const TGAColor red(255, 0, 0, 255);
	const TGAColor green(0, 255, 0, 255);
	const TGAColor blue(0, 0, 255, 255);
	TGAProcessor processor(100, 100, TGAProcessor::RGB);
	processor.set(50, 40, red);
	//processor.set(60, 60, green);
	//processor.set(40, 40, blue);
	//processor.set(30, 30, white);
	processor.flip_vertically();
	const char* filename("test_tga_1.tga");
	processor.write_tga_file(filename);
	std::cout << "Write file complete." << filename << std::endl;

	return 0;
}
#endif // TEST_TGA
