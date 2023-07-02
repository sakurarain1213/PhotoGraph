#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <string.h>
#include <opencv2/opencv.hpp>
using namespace cv;

namespace PhotoGraph {
	enum Format {
		GRAYSCALE = 1,
		RGB = 3,
		RGBA = 4
	};

	struct Color {
		union {
			struct {
				unsigned char r, g, b, a;
			};
			unsigned char raw[4];
			unsigned int val;
		};
		unsigned char bytespp;
		unsigned char& operator [] (const int& idx) {
			return raw[idx];
		}
		Color& operator * (float f) {
			Color out;
			for (int i = 0; i < bytespp; ++i)
				out[i] = raw[i] * f;
			return out;
		}
		Color& operator = (const Color& c) {
			if (this != &c) {
				val = c.val;
			}
			return *this;
		}
		Color() : val(0), bytespp(1) {}
		Color(int _r, int _g, int _b) : r(_r), g(_g), b(_b), bytespp(3) {}
		Color(int _r, int _g, int _b, int _a) : r(_r), g(_g), b(_b), a(_a), bytespp(4) {}
		Color(int v, unsigned char bpp) : val(v), bytespp(bpp) {}
		Color(unsigned char* p, unsigned char bpp) : bytespp(bpp) {
			for (int i = 0;i < bpp;++i)
				raw[i] = p[i];
		}
	};

	class Texture {
	private:	
		unsigned char* data;
		int pixelHeight;
		int pixelWidth;
		unsigned char bytespp;
		Vec4f averageRGB;

	public:
		Texture(int height = 1, int width = 1, int bytespp = 3) : pixelHeight(height), pixelWidth(width), bytespp(bytespp) {
			data = new unsigned char[height * width * bytespp];
			memset(data, 0xff, height * width * bytespp * sizeof(unsigned char));
		}
		Texture(const char* filename) {
			Mat out = imread(filename);
			pixelHeight = out.rows;
			pixelWidth = out.cols;
			bytespp = out.channels();
			data = new unsigned char[pixelHeight * pixelWidth * bytespp];
			memcpy_s(data, pixelHeight * pixelWidth * bytespp, out.data, pixelHeight * pixelWidth * bytespp);
			averageRGB = calculateAverageRGB();
		}
		Color get(int x, int y) {
			if (x < 0 || y < 0 || x >= pixelWidth || y >= pixelHeight)
				return Color();
			return Color(data + (x + y * pixelWidth) * bytespp, bytespp);
		}
		bool set(int x, int y, Color& c) {
			if (!data || x < 0 || y < 0 || x >= pixelWidth || y >= pixelHeight)
				return 0;
			memcpy(data + (x + y * pixelWidth) * bytespp, c.raw, bytespp);
			return 1;
		}
		Mat toMat() {
			int sz[2] = { pixelHeight, pixelWidth };
			if (bytespp == GRAYSCALE)
				return cv::Mat(2, sz, CV_8UC1, data);
			if (bytespp == RGB)
				return cv::Mat(2, sz, CV_8UC3, data);
			if (bytespp == RGBA)
				return cv::Mat(2, sz, CV_8UC4, data);
		}
		inline int getPixelHeight() { return pixelHeight; }
		inline int getPixelWidth() { return pixelWidth; }
		inline int getBytespp() { return bytespp; }
		inline unsigned char* getData() { return data; }

		Vec4f calculateAverageRGB() {
			Vec4f avg(0, 0, 0, 0);
			int count = pixelWidth * pixelHeight;
				
			for (int i = 0; i < pixelWidth; i++) {
				for (int j = 0; j < pixelHeight; j++) {
					unsigned char * p=data + (i + j * pixelWidth) * bytespp;
					avg[0] += p[0];
					avg[1] += p[1];
					avg[2] += p[2];
				}
			}
			if (count > 0) {
				avg[0] /= count;
				avg[1] /= count;
				avg[2] /= count;
			}
			return avg;
		}
		Vec4f getAverageRGB() {
			return averageRGB;
		}


	};
}

#endif

