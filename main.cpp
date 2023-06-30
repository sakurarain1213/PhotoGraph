#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "pass.h"

using namespace PhotoGraph;

int main() {
	Pass examplePass;
	examplePass.defineNode<Node_Texture>("Texture1");
	Node_Texture* texture1 = examplePass.getNode<Node_Texture>("Texture1");
	Texture exampleTex("<file name here>");
	texture1->tex = &exampleTex;
	examplePass.defineNode<Node_Sample_Texture>("Sample Texture");
	examplePass.defineNode<Node_Output>("Output");
	Node_Output* output = examplePass.getNode<Node_Output>("Output");
	output->height = 512;
	output->width = 512;
	examplePass.bind("Texture1", "Tex", "Sample Texture", "Tex");
	examplePass.bind("Sample Texture", "Out", "Output", "In");
	examplePass.sequenceGeneration();
	examplePass.work();
	cv::imshow("Example Output", examplePass.getTexture()->toMat());
	return 0;
}
