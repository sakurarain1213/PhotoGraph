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
		virtual void definePorts() {} /*����������ж�������˿ں�����˿�*/
		virtual void work(RuntimeInformation rinfo) {} /*����������*/
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
		Color c; /*��ʾ����������*/
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

	/*RGB��ת*/
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


	/*RGBת�Ҷ�*/
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
			/*���鹫ʽ*/
			setOutput<float>("Out", t);
		}
	};


	/*�Ҷ�תRGB �������*/
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


	/*�Ҷ�ͼ��ֵ��*/
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

	/*ƽ��*/
	class Node_Move : public Node {
		private:
		float X_Offset = 0.5;
		float Y_Offset = 0.5;
		//Ĭ�� ǰ�˿ɸ�
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





	/*��ת  ����ת  ͸�ӱ任 ����任*/


	/*��Ե��� ������ȡ   */


	/*��ֵ�˲�    */   /*��ʴ  /*����*/


	/*��     */


	/*�˲����ڵ㣺ʵ�ָ���ͼ���˲��������ֵ�˲�����˹�˲�����ֵ�˲��ȡ�
	��Ե���ڵ㣺ʵ�ֱ�Ե����㷨����Sobel���ӡ�Canny���ӵȡ�
	��������/�ԱȶȽڵ㣺ͨ������ͼ������ȺͶԱȶ����ı�ͼ�����ۡ�
	����ɫ�ʽڵ㣺ʵ����ɫУ����ɫ��ƽ�⡢���Ͷȵ����ȹ��ܡ�
	ͼ��任�ڵ㣺ʵ��ͼ�����ת�����š���ת�ȱ任������
	��̬ѧ����ڵ㣺ʵ�����͡���ʴ�������㡢���������̬ѧ���������
	ͼ��ָ�ڵ㣺ʵ��ͼ��ָ��㷨������ֵ�ָ�����������߽���ȡ�ȡ�
	������ȡ�ڵ㣺ʵ��ͼ��������ȡ�㷨����ǵ��⡢��Ե��ȡ�����������ȡ�
	Ƶ����ڵ㣺ʵ�ָ���Ҷ�任��Ƶ���˲���Ƶ��������ȡ��Ƶ���������
	ͼ��ϳɽڵ㣺ʵ��ͼ��ϳ��㷨����ͼ���ںϡ�ͼ���ɰ桢ͼ����ӵȡ�
	*/









}

#endif