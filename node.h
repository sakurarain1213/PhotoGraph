#pragma once

#ifndef _NODE_H
#define _NODE_H

#include "port.h"
#include "vec.h"
#include "texture.h"
#include <set>

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


	/*RGB转灰度*/
	class Node_RGB2Grayscale : public Node {
	public:
		Node_RGB2Grayscale() {}
		virtual void definePorts() {
			defineInputPort<Vec4f>("In");
			defineOutputPort<int>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec4f v = getInput<Vec4f>("In");
			float t = 0.299 * v.r + 0.587 * v.g + 0.114 * v.b;
			/*经验公式*/
			setOutput<int>("Out", (int)t);
		}
	};


	/*灰度图二值化*/
	class Node_Binarization : public Node {
	public:
		Node_Binarization() {}
		virtual void definePorts() {
			defineInputPort<int>("In");
			defineOutputPort<int>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			int v = getInput<int>("In");
			if (v > 127)  v = 255;
			else v = 0;
			setOutput<int>("Out", v);
		}
	};

	/*平移 旋转  镜像翻转（要参数）*/
	class Node_Move : public Node {
	public:
		Node_Move() {}

		virtual void definePorts() {
			defineInputPort<Vec4f>("In");
			defineOutputPort<Vec4f>("Out");
		}
		virtual void work(RuntimeInformation rinfo) {
			Vec4f v = getInput<Vec4f>("In");


			//保存成图片
			//得到相对位置   从图片里找

			setOutput<Vec4f>("Out", v);
		}
	};





	/*透视变换 仿射变换*/


	/*边缘检测 轮廓提取   */


	/*中值滤波    */   /*腐蚀  /*膨胀*/


	/*锐化     */












}

#endif