#pragma once

#ifndef _NODE_H
#define _NODE_H

#include "port.h"
#include "vec.h"
#include "texture.h"
#include <set>
#include <random>

namespace PhotoGraph {
	class Pass;

	struct RuntimeInformation {
		Vec2f uv0;
		Vec2i screenPosition;
	};

	class Node {
	private:
		InputPortMap ipm;
		OutputPortMap opm;
	protected:
		template <typename T>
		inline T getInput(std::string port_name) {
			return ipm.getInput<T>(port_name);
		}
		template <typename T>
		inline void setOutput(std::string port_name, T value) {
			return opm.setOutput<T>(port_name, value);
		}
		inline bool isBinded(std::string port_name) {
			return ipm.getInputPort<int>(port_name)->isBinded();
		}
		template <typename T>
		inline void defineInputPort(std::string port_name) {
			ipm.defineInputPort<T>(port_name);
		}
		template <typename T>
		inline void defineOutputPort(std::string port_name) {
			opm.defineOutputPort<T>(port_name);
		}
	public:
		virtual void definePorts() {} /*在这个函数中定义输入端口和输出端口*/
		virtual void work(RuntimeInformation rinfo) {} /*定义具体操作*/
		std::set<Node*> binded_set;
		std::set<Node*> dependency_set;
		Node() { definePorts(); }
		void bind(std::string output_port, Node* input_node, std::string input_port) {
			input_node->ipm.getInputPort<int>(input_port)->bind(opm.getOutputPort<int>(output_port));
			binded_set.insert(input_node);
			input_node->dependency_set.insert(this);
		}
	};

	class Node_Output : public Node {
	public:
		int width;
		int height;
		Color c; /*表示最后的输出结果*/
		Node_Output() {}
		virtual void definePorts() {
			defineInputPort<Vec4f>("In");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec4f in = getInput<Vec4f>("In");
			c = Color(in.r, in.g, in.b, in.a);
		}
	};

	class Node_Texture : public Node {
	public:
		Texture* tex;
		Node_Texture() {}
		virtual void definePorts() {
			defineOutputPort<Texture*>("Tex");
		}
		virtual void work(RuntimeInformation rinfo) {
			setOutput<Texture*>("Tex", tex);
		}
	};

	class Node_Sample_Texture : public Node {
	public:
		Node_Sample_Texture() {}
		virtual void definePorts() {
			defineInputPort<Texture*>("Tex");
			defineInputPort<Vec2f>("UV");
			defineOutputPort<Vec4f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec2f uv;
			if (!isBinded("UV")) {
				uv = rinfo.uv0;
			}
			else {
				uv = getInput<Vec2f>("UV");
			}
			Texture* tex = getInput<Texture*>("Tex");
			Color c = tex->get(uv.u * tex->getPixelWidth(), uv.v * tex->getPixelHeight());
			setOutput<Vec4f>("Out", Vec4f(c.r, c.g, c.b, c.a));
		}
	};


	/*提供UV
	class Node_UV : public Node {
	public:
		Vec2f uv;
		Node_UV() {}
		virtual void definePorts() {
			defineOutputPort<Vec2f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			setOutput<Vec2f>("Out", uv);
		}
	};
	*/






	/*RGB翻转*/
	class Node_Inverse : public Node {
	public:
		Node_Inverse() {}
		virtual void definePorts() {
			defineInputPort<Vec4f>("In");
			defineOutputPort<Vec4f>("Out");
		}

		virtual void work(RuntimeInformation rinfo) {
			Vec4f v = getInput<Vec4f>("In");
			v.r = 255 - v.r;
			v.g = 255 - v.g;
			v.b = 255 - v.b;
			setOutput<Vec4f>("Out", v);
		}
	};

	/*对比度调整  需要threshold*/
	class Node_AdjustContrast : public Node {
	private:
		float contrast=0.05;  //+-0.1
	public:
		Node_AdjustContrast() {}
		virtual void definePorts() {
			defineInputPort<Vec4f>("In");
			defineInputPort<Texture*>("TexIn");
			defineOutputPort<Vec4f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec4f inputColor = getInput<Vec4f>("In");
			Texture* Tex = getInput<Texture*>("TexIn");

			Vec4f adjustedColor;
			Vec4f avg= Tex->getAverageRGB();

			if (contrast != 1.0) {
				// 计算对比度调整的因子
				float factor = (contrast + 1) / (1 - contrast);
				adjustedColor.r = factor * (inputColor.r - avg.r) + avg.r;
				adjustedColor.g = factor * (inputColor.g - avg.r) + avg.r;
				adjustedColor.b = factor * (inputColor.b - avg.r) + avg.r;
			}
			else {
				adjustedColor = inputColor;
			}
			setOutput<Vec4f>("Out", adjustedColor);
		}
	};

	/*饱和度 需要threshold*/
	class Node_Saturation : public Node {
	private:
		float saturation=1.05;   //0~1变灰  1+过饱和
	public:
		Node_Saturation() {}
		virtual void definePorts() {
			defineInputPort<Vec4f>("In");
			defineOutputPort<Vec4f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec4f inputColor = getInput<Vec4f>("In");
			float r, g, b;
			if (saturation < 1) {
				float gray = (inputColor.r + inputColor.g + inputColor.b) / 3;
				 r = lerp(gray, inputColor.r, saturation);
				 g = lerp(gray, inputColor.g, saturation);
				 b = lerp(gray, inputColor.b, saturation);
			}
			else {
				 r = inputColor.r * saturation;
				 g = inputColor.g * saturation;
				 b = inputColor.b * saturation;
			}

			setOutput<Vec4f>("Out", Vec4f(r, g, b, inputColor.a));
		}
		// 简单线性插值
		float lerp(float a, float b, float t) {
			return a * (1 - t) + b * t;
		}
	};



	/*RGB转灰度*/
	class Node_RGB2Grayscale : public Node {
	public:
		Node_RGB2Grayscale() {}
		virtual void definePorts() {
			defineInputPort<Vec4f>("In");
			defineOutputPort<float>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec4f v = getInput<Vec4f>("In");
			float t = 0.299 * v.r + 0.587 * v.g + 0.114 * v.b;
			/*经验公式*/
			setOutput<float>("Out", t);
		}
	};


	/*灰度转RGB 用于输出*/
	class Node_Gray2RGB : public Node {
	public:
		Node_Gray2RGB() {}
		virtual void definePorts() {
			defineInputPort<float>("In");
			defineOutputPort<Vec4f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			float t = getInput<float>("In");
			Vec4f v;
			v.r = v.g = v.b = t;
			v.a = 100;
			setOutput<Vec4f>("Out", v);
		}
	};


	/*灰度图二值化*/
	class Node_Binarization : public Node {
	public:
		Node_Binarization() {}
		virtual void definePorts() {
			defineInputPort<float>("In");
			defineOutputPort<float>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			float v = getInput<float>("In");
			if (v > 127.0)  v = 255.0;
			else v = 0.0;
			setOutput<float>("Out", v);
		}
	};

	/*平移*/
	class Node_Move : public Node {
		private:
		float X_Offset = 0.5;
		float Y_Offset = 0.5;
		//默认 前端可改
	public:
		Node_Move() {}

		virtual void definePorts() {
			defineInputPort<Vec2f>("UV");
			defineOutputPort<Vec2f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec2f input;
			if (!isBinded("UV")) {
				input = rinfo.uv0;
			}
			else {
				input = getInput<Vec2f>("UV");
			}
			Vec2f output; 
			output.u= input.u - X_Offset;
			output.v= input.v - Y_Offset;
			setOutput<Vec2f>("Out", output);
		}
	};


	/*矩阵乘算 不传UV  权值为0  提取值方向的花纹   权值+ 高亮曝光*/
	class Node_Matrix3_Test : public Node {
	private:
		Matrix3x3 operat;
		float f1=0;   float f2=-2;   float f3=0;
		float f4=0;   float f5=4;   float f6=0;
		float f7=0;   float f8=-2;   float f9=0;

	public:
		Node_Matrix3_Test() {}
		virtual void definePorts() {
			defineInputPort<Texture*>("Tex");
			defineInputPort<Vec2f>("UV");
			defineOutputPort<Vec4f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec2f uv;
			if (!isBinded("UV")) {
				uv = rinfo.uv0;
			}
			else uv = getInput<Vec2f>("UV");
			Texture* tex = getInput<Texture*>("Tex");
			
			Vec4f output;
			
			/*M initial
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					operat.raw[i][j] = 1.0;
				}
			}*/
			initialM(f1, f2, f3,
					f4, f5, f6,
					f7, f8, f9
			);

			std::vector<Color> colors;
			/*colors  012  345  678*/

			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {

					Color c = tex->get(uv.u * tex->getPixelWidth()  +i  , uv.v * tex->getPixelHeight()+j);
					//绝对坐标=uv*长宽
					colors.push_back(c);
				}
			}
			//calculate
			output.r = output.g = output.b = 0; output.a = 100;
			int cnt = 0;
			for (int i = 0; i <3; i++) {
				for (int j = 0; j <3; j++) {
					Vec4f temp;
					temp.r = colors[cnt].raw[0];
					temp.g = colors[cnt].raw[1];
					temp.b = colors[cnt].raw[2];

					//output = output + temp * operat.raw[i][j];
					output.r = output.r + temp.r * operat.raw[i][j];
					output.g = output.g + temp.g * operat.raw[i][j];
					output.b = output.b + temp.b * operat.raw[i][j];
					cnt++;
					
				}
			}
			//printf("%f   ", output.b);
			//校正
			for (int i = 0; i < 3; i++){
				if (output.raw[i] > 255) output.raw[i] = 255;
				else if (output.raw[i] <  0)output.raw[i] = 0;
			}
	
			output.a = colors[5].raw[3];//中心点的a
			//printf("%f", colors[5].raw[2]); 
			setOutput <Vec4f> ("Out", output);
			
		}
		virtual void initialM(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9) {
			operat.raw[0][0] = f1; operat.raw[0][1] = f2; operat.raw[0][2] = f3;
			operat.raw[1][0] = f4; operat.raw[1][1] = f5; operat.raw[1][2] = f6;
			operat.raw[2][0] = f7; operat.raw[2][1] = f8; operat.raw[2][2] = f9;
		}
	
	};


	/*9*9模糊*/
	class Node_Matrix9_Avg : public Node {
	public:
		Node_Matrix9_Avg() {}
		virtual void definePorts() {
			defineInputPort<Texture*>("Tex");
			defineInputPort<Vec2f>("UV");
			defineOutputPort<Vec4f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec2f uv;
			if (!isBinded("UV")) {
				uv = rinfo.uv0;
			}
			else uv = getInput<Vec2f>("UV");
			Texture* tex = getInput<Texture*>("Tex");
			std::vector<Color> colors;
			/*colors  012  345  678*/
			for (int i = -4; i <= 4; i++) {
				for (int j = -4; j <= 4; j++) {
					Color c = tex->get(uv.u * tex->getPixelWidth() + i, uv.v * tex->getPixelHeight() + j);
					colors.push_back(c);
				}
			}
			//calculate
			Vec4f sum; sum.r =sum.g = sum.b = 0; sum.a = 100;
			for (int i = 0; i < 81; i++) {
				sum.r = sum.r + colors[i].r;
				sum.g = sum.g + colors[i].g;
				sum.b = sum.b + colors[i].b;
			}
			sum.r = sum.r / 81;
			sum.g = sum.g / 81;
			sum.b = sum.b / 81;
			sum.a = colors[41].a;//中心点的a
			setOutput <Vec4f>("Out", sum);
		}
	};


	/*3*3  中(均)值滤波  去噪    */
	class Node_MedianFilter : public Node {
	public:
		Node_MedianFilter() {}
		virtual void definePorts() {
			defineInputPort<Texture*>("Tex");
			defineInputPort<Vec2f>("UV");
			defineOutputPort<Vec4f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec2f uv;
			if (!isBinded("UV")) {
				uv = rinfo.uv0;
			}
			else uv = getInput<Vec2f>("UV");
			Texture* tex = getInput<Texture*>("Tex");

			Vec4f output;

			std::vector<Color> colors;

			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					Color c = tex->get(uv.u * tex->getPixelWidth() + i, uv.v * tex->getPixelHeight() + j);
					colors.push_back(c);
				}
			}
			float sumR = 0.0;
			float sumG = 0.0;
			float sumB = 0.0;
			for (const Color& c : colors) {
				sumR += c.raw[0]; 
				sumG += c.raw[1]; 
				sumB += c.raw[2];
			}
			int numColors = colors.size();
			float avgR = sumR / numColors; 
			float avgG = sumG / numColors; 
			float avgB = sumB / numColors; 

			output.r = avgR;
			output.g = avgG;
			output.b = avgB;
			output.a = colors[5].a;
			setOutput<Vec4f>("Out", output);
		}
	};

	/*随机椒盐噪声 */
	class Node_SaltAndPepperNoise : public Node {
	private:
		float p = 0.05;//噪声概率
	public:
		Node_SaltAndPepperNoise() {}
		virtual void definePorts() {
			defineInputPort<Texture*>("Tex");
			defineInputPort<Vec2f>("UV");
			defineOutputPort<Vec4f>("Out");
		}
	
		void addSaltAndPepperNoise(Vec4f &c, float saltProbability, float pepperProbability) {
			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_real_distribution<float> distribution(0.0, 1.0);
			float randValue = distribution(generator);
			if (randValue < saltProbability) {
				// 盐噪 max
				c.raw[0] = 255;
				c.raw[1] = 255;
				c.raw[2] = 255;
				//printf("yes");
			}
			else if (randValue > 1.0 - pepperProbability) {
				// 椒噪 min
				c.raw[0] = 0.0;
				c.raw[1] = 0.0;
				c.raw[2] = 0.0;
			}
		}

		virtual void work(RuntimeInformation rinfo) {
			Vec2f uv;
			if (!isBinded("UV")) {
				uv = rinfo.uv0;
			}
			else {
				uv = getInput<Vec2f>("UV");
			}
			Texture* tex = getInput<Texture*>("Tex");
			Vec4f output;
			Color c = tex->get(uv.u * tex->getPixelWidth(), uv.v * tex->getPixelHeight());
			output.r = c.raw[0]; output.g = c.raw[1]; output.b = c.raw[2];
			addSaltAndPepperNoise(output, p, p); // 加 p概率的椒盐噪声
			output.a = c.a;
			setOutput<Vec4f>("Out", output);
		}

	};


	/*用于噪声的功能类 柏林噪声*/
	class PerlinNoise {
	private:
		static const int permutationTableSize = 256;
		int permutation[permutationTableSize * 2];
	public:
		PerlinNoise() {
			// Initialize the permutation table with pseudo-random values
			for (int i = 0; i < permutationTableSize; i++) {
				permutation[i] = i;
			}
			// Shuffle the permutation table
			for (int i = 0; i < permutationTableSize; i++) {
				int j = rand() % permutationTableSize;
				std::swap(permutation[i], permutation[j]);
				permutation[i + permutationTableSize] = permutation[i];
			}
		}
		float noise(float x, float y) const {
			// Determine the grid cell coordinates
			int X = static_cast<int>(std::floor(x)) & (permutationTableSize - 1);
			int Y = static_cast<int>(std::floor(y)) & (permutationTableSize - 1);
			// Determine the relative x, y coordinates within the grid cell
			x -= std::floor(x);
			y -= std::floor(y);
			// Compute fade curves for x, y
			float fadeX = fade(x);
			float fadeY = fade(y);
			// Hash coordinates of the 4 cube corners
			int A = permutation[X] + Y;
			int B = permutation[X + 1] + Y;
			// And add blended results from 4 corners of the cube
			float u = grad(permutation[A], x, y);
			float v = grad(permutation[B], x - 1, y);
			float lerpX1 = lerp(u, v, fadeX);
			A = permutation[X] + Y + 1;
			B = permutation[X + 1] + Y + 1;
			u = grad(permutation[A], x, y - 1);
			v = grad(permutation[B], x - 1, y - 1);
			float lerpX2 = lerp(u, v, fadeX);
			return (lerp(lerpX1, lerpX2, fadeY) + 1.0) / 2.0;
		}
	private:
		inline float fade(float t) const {
			return t * t * t * (t * (t * 6 - 15) + 10);
		}
		inline float lerp(float a, float b, float t) const {
			return a + t * (b - a);
		}
		inline float grad(int hash, float x, float y) const {
			int h = hash & 15;
			float u = h < 8 ? x : y;
			float v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);
			return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
		}
	};



	/*均匀噪声  柏林噪声*/
	class Node_PerlinNoise : public Node {
	private:
		float scale = 1.0;// 噪声尺度
	public:
		Node_PerlinNoise() {}

		virtual void definePorts() {
			defineInputPort<Vec2f>("UV");
			defineInputPort<Texture*>("Tex");
			defineOutputPort<Vec4f>("Out");

		}

		virtual void work(RuntimeInformation rinfo) {
			Vec2f uv;
			if (!isBinded("UV")) {
				uv = rinfo.uv0;
			}
			else {
				uv = getInput<Vec2f>("UV");
			}

			Texture* tex = getInput<Texture*>("Tex");
			Color c = tex->get(uv.u * tex->getPixelWidth(), uv.v * tex->getPixelHeight());
			Vec4f output;

			PerlinNoise noise; // 创建PerlinNoise对象
			float noiseValue = noise.noise(scale * uv.u, scale * uv.v);//只需要比例坐标

			//debug
			output.r = noiseValue;
			output.a =c.raw[3];
			setOutput<Vec4f>("Out", output);
		}

	};


	



	/* 最值膨胀（白而糊）  彩色版*/
	class Node_Dilation : public Node {
	
	private:
		int core=3; //2*core+1即卷积核边长
	public:
		Node_Dilation() {}
		virtual void definePorts() {
			defineInputPort<Texture*>("Tex");
			defineInputPort<Vec2f>("UV");
			defineOutputPort<Vec4f>("Out");
		}

		virtual void work(RuntimeInformation rinfo) {
			Vec2f uv;
			if (!isBinded("UV")) {
				uv = rinfo.uv0;
			}
			else {
				uv = getInput<Vec2f>("UV");
			}
			Texture* tex = getInput<Texture*>("Tex");

			Vec4f output;
			std::vector<Color> colors;

			// 3x3
			for (int i = -core; i <= core; i++) {
				for (int j = -core; j <= core; j++) {
					Color c = tex->get(uv.u * tex->getPixelWidth() + i, uv.v * tex->getPixelHeight() + j);
					colors.push_back(c);
				}
			}

			// Find RGB max color 
			auto maxColorR = std::max_element(colors.begin(), colors.end(), [](const Color& a, const Color& b) {
				return a.r < b.r;
				});
			auto maxColorG = std::max_element(colors.begin(), colors.end(), [](const Color& a, const Color& b) {
				return a.g < b.g;
				});
			auto maxColorB = std::max_element(colors.begin(), colors.end(), [](const Color& a, const Color& b) {
				return a.b < b.b;
				});

			output.r = maxColorR->r;
			output.g = maxColorG->g;
			output.b = maxColorB->b;
			output.a = colors[2*core*core+2*core].a;//卷积核中心点

			setOutput<Vec4f>("Out", output);
		}
	};

	/*最值腐蚀（黑而糊） 彩色版*/
	class Node_Erosion : public Node {
	private:
		int core = 3; // 2*core+1即卷积核边长
	public:
		Node_Erosion() {}
		virtual void definePorts() {
			defineInputPort<Texture*>("Tex");
			defineInputPort<Vec2f>("UV");
			defineOutputPort<Vec4f>("Out");
		}

		virtual void work(RuntimeInformation rinfo) {
			Vec2f uv;
			if (!isBinded("UV")) {
				uv = rinfo.uv0;
			}
			else {
				uv = getInput<Vec2f>("UV");
			}
			Texture* tex = getInput<Texture*>("Tex");

			Vec4f output;
			std::vector<Color> colors;

			// 3x3
			for (int i = -core; i <= core; i++) {
				for (int j = -core; j <= core; j++) {
					Color c = tex->get(uv.u * tex->getPixelWidth() + i, uv.v * tex->getPixelHeight() + j);
					colors.push_back(c);
				}
			}

			// Find RGB min color 
			auto minColorR = std::min_element(colors.begin(), colors.end(), [](const Color& a, const Color& b) {
				return a.r < b.r;
				});
			auto minColorG = std::min_element(colors.begin(), colors.end(), [](const Color& a, const Color& b) {
				return a.g < b.g;
				});
			auto minColorB = std::min_element(colors.begin(), colors.end(), [](const Color& a, const Color& b) {
				return a.b < b.b;
				});

			output.r = minColorR->r;
			output.g = minColorG->g;
			output.b = minColorB->b;

			output.a = colors[2 * core * core + 2 * core].a; // 卷积核中心点

			setOutput<Vec4f>("Out", output);
		}
	};


	/*边缘检测  轮廓提取 彩色*/
	class Node_EdgeDetection : public Node {
	private:
		int core = 1; // 2*core+1即卷积核边长
	public:
		Node_EdgeDetection() {}
		virtual void definePorts() {
			defineInputPort<Texture*>("Tex");
			defineInputPort<Vec2f>("UV");
			defineOutputPort<Vec4f>("Out");
		}

		virtual void work(RuntimeInformation rinfo) {
			Vec2f uv;
			if (!isBinded("UV")) {
				uv = rinfo.uv0;
			}
			else {
				uv = getInput<Vec2f>("UV");
			}
			Texture* tex = getInput<Texture*>("Tex");

			Vec4f output;
			std::vector<Color> colors;

			// 3x3
			for (int i = -core; i <= core; i++) {
				for (int j = -core; j <= core; j++) {
					Color c = tex->get(uv.u * tex->getPixelWidth() + i, uv.v * tex->getPixelHeight() + j);
					colors.push_back(c);
				}
			}
			// Calculate abs  between center and neighbors
			Color centerColor = colors[core * (2 * core + 1) + core];
			float diffR = 0.0;
			float diffG = 0.0;
			float diffB = 0.0;
			for (const Color& c : colors) {
				diffR += std::abs(c.r - centerColor.r);
				diffG += std::abs(c.g - centerColor.g);
				diffB += std::abs(c.b - centerColor.b);
			}
			output.r = diffR;
			output.g = diffG;
			output.b = diffB;
			output.a = centerColor.a;
			setOutput<Vec4f>("Out", output);
		}
	};





	class Math_Node : public Node {
	public:
		Math_Node() {}
		// 共享的功能或属性
		void computeAllNodes() {
			// 遍历所有绑定的数学节点 依次执行它们的计算
			for (Node* node : binded_set) {
				Math_Node* mathNode = dynamic_cast<Math_Node*>(node);
				if (mathNode) {
					mathNode->compute();
				}
			}
		}
		//虚方法 执行数学节点的计算
		virtual void compute() = 0;
	};

	/*无参生成随机float*/
	class Node_RandomFloat : public Math_Node {
	public:
		virtual void definePorts() {
			defineOutputPort<float>("Out");
		}
		virtual void  work(RuntimeInformation rinfo) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> dis(0.0f,255.0f);
			setOutput<float>("Out", dis(gen));
		}
	};

	/*float*Vec4f*/
	class Node_floatXVec4f : public Math_Node {
	public:
		virtual void definePorts() {
			defineInputPort<Vec4f>("Vec4fIn");
			defineInputPort<float>("floatIn");
			defineOutputPort< Vec4f >("Out");
		}
		virtual void  work(RuntimeInformation rinfo) {
			Vec4f v=getInput<Vec4f>("Vec4fIn");
			float f= getInput<float>("floatIn");
			v.r *= f; v.g *= f; v.b *= f; 
			setOutput<Vec4f>("Out",v);
		}
	};

	class Node_Threshold : public Node {
	public:
		Node_Threshold() {}

		virtual void definePorts() {
			defineInputPort<cv::Vec4f>("In");
			defineOutputPort<cv::Vec4f>("Out");
		}

		virtual void work(RuntimeInformation rinfo) {
			Vec4f inputVec4f = getInput<Vec4f>("In");
			inputVec4f.r > 255 ? 255 : inputVec4f.r;
			inputVec4f.r <  0  ?   0 : inputVec4f.r;
			inputVec4f.g > 255 ? 255 : inputVec4f.g;
			inputVec4f.g < 0 ? 0 : inputVec4f.g;
			inputVec4f.b > 255 ? 255 : inputVec4f.b;
			inputVec4f.b < 0 ? 0 : inputVec4f.b;
			setOutput<Vec4f>("Out", inputVec4f);
		}
	};

	/*Vec4f*Matrix*/
	class Node_Vec4fXMatrix : public Math_Node {
	public:
		virtual void definePorts() {
			defineInputPort<Vec4f>("Vec4fIn");
			defineInputPort<Matrix4x4>("MatIn");
			defineOutputPort<Vec4f>("Out");
		}
		virtual void  work(RuntimeInformation rinfo) {
			Vec4f v = getInput<Vec4f>("Vec4fIn");
			Matrix4x4 M = getInput<Matrix4x4>("MatIn");
			Vec4f result;
			result.r = M.raw[0].r * v.r + M.raw[1].r * v.g + M.raw[2].r * v.b + M.raw[3].r * v.a;
			result.g = M.raw[0].g * v.r + M.raw[1].g * v.g + M.raw[2].g * v.b + M.raw[3].g * v.a;
			result.b = M.raw[0].b * v.r + M.raw[1].b * v.g + M.raw[2].b * v.b + M.raw[3].b * v.a;
			result.a = M.raw[0].a * v.r + M.raw[1].a * v.g + M.raw[2].a * v.b + M.raw[3].a * v.a;
			setOutput<Vec4f>("Out", result);
		}
	};


	/*通道混合
	Mat dst = Mat::zeros(image.size(), image.type());
	int ft[] = { 0,2,1,1,2,0 };//互换1、3通道
	mixChannels(&image,1, &dst,1, ft,3);
	imshow("Mix", dst);

	*/

	/*旋转  镜像翻转  透视变换 仿射变换*/






	/*锐化     */


	/*滤波器节点：实现各种图像滤波器，如均值滤波、高斯滤波、中值滤波等。
	边缘检测节点：实现边缘检测算法，如Sobel算子、Canny算子等。
	调整亮度/对比度节点：通过调整图像的亮度和对比度来改变图像的外观。
	调整色彩节点：实现颜色校正、色彩平衡、饱和度调整等功能。
	图像变换节点：实现图像的旋转、缩放、翻转等变换操作。
	形态学处理节点：实现膨胀、腐蚀、开运算、闭运算等形态学处理操作。
	图像分割节点：实现图像分割算法，如阈值分割、区域生长、边界提取等。
	特征提取节点：实现图像特征提取算法，如角点检测、边缘提取、纹理特征等。
	频域处理节点：实现傅里叶变换、频域滤波、频域特征提取等频域处理操作。
	图像合成节点：实现图像合成算法，如图像融合、图像蒙版、图像叠加等。
	*/









}

#endif