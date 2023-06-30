#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "pass.h"

using namespace PhotoGraph;

int main() {
	Pass examplePass;

	examplePass.defineNode<Node_Texture>("Texture1");
	examplePass.defineNode<Node_Sample_Texture>("Sample Texture");
	examplePass.defineNode<Node_Output>("Output");
	//examplePass.defineNode<Node_Inverse>("Inverse");
	//名字任取 作为标识  注册了强制要使用

	/*
    examplePass.defineNode<Node_RGB2Grayscale>("RGB2Gray");
	examplePass.defineNode<Node_Gray2RGB>("Gray2RGB");
	examplePass.defineNode<Node_Binarization>("Binary");
	examplePass.defineNode<Node_Move>("Move");
	*/

	Node_Texture* texture1 = examplePass.getNode<Node_Texture>("Texture1");
	Texture exampleTex("C:\\Users\\w1625\\Desktop\\computer_picture\\0.jpg");
	texture1->tex = &exampleTex;
	Node_Output* output = examplePass.getNode<Node_Output>("Output");


	output->height = 512;
	output->width = 1024;

	examplePass.bind("Texture1", "Tex", "Sample Texture", "Tex");

	examplePass.bind("Sample Texture", "Out", "Output", "In");



	/*
	examplePass.bind("Texture1", "Tex", "Sample Texture", "Tex");
	examplePass.bind("Sample Texture", "Out", "Inverse", "In");
	examplePass.bind("Inverse", "Out", "Output", "In");
	*/

	/*
	examplePass.bind("Texture1", "Tex", "Sample Texture", "Tex");//平移要在有UV端口的地方输入
	examplePass.bind("Move", "Out", "Sample Texture", "UV");
	examplePass.bind("Sample Texture", "Out", "RGB2Gray", "In");
	examplePass.bind("RGB2Gray", "Out", "Binary", "In");
	examplePass.bind("Binary", "Out", "Gray2RGB", "In");
	examplePass.bind("Gray2RGB", "Out", "Output", "In");
	*/
	examplePass.sequenceGeneration();
	examplePass.work();
	cv::imshow("Example Output", examplePass.getTexture()->toMat());
	cv::waitKey(0);
	return 0;
}
