#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include "pass.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <vector>

using namespace rapidjson;
using namespace std;
using namespace PhotoGraph;


void createNode(string type, string name,Pass& examplePass, vector<std::string >ss)
{
	if (type == "Output") {
		examplePass.defineNode<Node_Output>(name,ss);
		examplePass.check();
	}
	else if (type == "Texture") {
		examplePass.defineNode<Node_Texture>(name, ss);
	}
	else if (type == "Sample Texture") {
		examplePass.defineNode<Node_Sample_Texture>(name, ss);
	}
	else if (type == "Inverse") {
		examplePass.defineNode<Node_Inverse>(name,ss);
	}
	else if (type == "AdjustContrast") {
		examplePass.defineNode<Node_AdjustContrast>(name, ss);
	}
	else if (type == "Saturation") {
		examplePass.defineNode<Node_Saturation>(name, ss);
	}
	else if (type == "RGB2Grayscale") {
		examplePass.defineNode<Node_RGB2Grayscale>(name, ss);
	}
	else if (type == "Gray2RGB") {
		examplePass.defineNode<Node_Gray2RGB>(name, ss);
	}
	else if (type == "Binarization") {
		examplePass.defineNode<Node_Binarization>(name, ss);
	}
	else if (type == "Move") {
		examplePass.defineNode<Node_Move>(name, ss);
	}
	else if (type == "Matrix3") {
		examplePass.defineNode<Node_Matrix3>(name, ss);

		//printf("create success:"); cout << name << "\n";
	}
	else if (type == "Matrix Sample") {
		examplePass.defineNode<Node_Matrix3_Sample>(name, ss);
		//printf("create success:"); cout << name << "\n";
	}
	else if (type == "Matrix9_Avg") {
		examplePass.defineNode<Node_Matrix9_Avg>(name, ss);
	}
	else if (type == "MedianFilter") {
		examplePass.defineNode<Node_MedianFilter>(name, ss);
	}
	else if (type == "Dilation") {
		examplePass.defineNode<Node_Dilation>(name, ss);
	}
	else if (type == "Erosion") {
		examplePass.defineNode<Node_Erosion>(name, ss);
	}
	else if (type == "EdgeDetection") {
		examplePass.defineNode<Node_EdgeDetection>(name, ss);
	}
	else {
		std::printf("error create\n"); 
		//或日志输出

	}
}



void bindNode(string N1, string out, string N2, string in, Pass& examplePass)
{
	examplePass.bind(N1, out, N2, in );
}


int main() {

	Pass examplePass;

	/*
	vector <string> s;
	examplePass.defineNode<Node_Texture>("Texture1",s);
	examplePass.defineNode<Node_Sample_Texture>("Sample Texture", s);
	examplePass.defineNode<Node_Output>("Output", s);
	examplePass.defineNode<Node_SaltAndPepperNoise>("N", s);
	Node_Texture* texture1 = examplePass.getNode<Node_Texture>("Texture1");
	Texture exampleTex("C:\\Users\\w1625\\Desktop\\11.jpg");
	texture1->tex = &exampleTex;
	Node_Output* output = examplePass.getNode<Node_Output>("Output");
	output->height = 512;
	output->width = 512;
	examplePass.bind("Texture1", "Tex", "Sample Texture", "Tex");
	examplePass.bind("Sample Texture", "Out", "N", "In");
	examplePass.bind("N", "Out", "Output", "In");

	examplePass.sequenceGeneration();
	int start_time = clock();
*/



	std::string jsonStr = R"({"name":"未命名","nodes":[{"name":"节点0","type":"Texture","position":{"x":329,"y":385},"attributes":[{"name":"Texture","type":"Tex","value":"C:\\Users\\w1625\\Desktop\\11.jpg"}]},{"name":"节点1","type":"Output","position":{"x":1034,"y":406},"attributes":[{"name":"Width","type":"1","value":["512"]},{"name":"Height","type":"1","value":["512"]}]},{"name":"节点2","type":"Sample Texture","position":{"x":700,"y":310},"attributes":[]},{"name":"节点3","type":"Move","position":{"x":409,"y":591},"attributes":[{"name":"X_Offset","type":"1","value":["0.12"]},{"name":"Y_Offset","type":"1","value":["0.1"]}]}],"links":[{"outNode":"节点3","outPort":"Out","inNode":"节点2","inPort":"UV"},{"outNode":"节点0","outPort":"Tex","inNode":"节点2","inPort":"Tex"},{"outNode":"节点2","outPort":"Out","inNode":"节点1","inPort":"In"}]})";
	rapidjson::Document document1;
	document1.Parse(jsonStr.c_str());

	/*
	if (document1.HasMember("name") && document1["name"].IsString()) {
		std::string value = document1["name"].GetString();
		std::cout << "Value ofname: " << value << std::endl;
	}
	*/
	if (document1.HasMember("nodes") && document1["nodes"].IsArray()) {
		 rapidjson::Value& nodesArray = document1["nodes"];
		for (rapidjson::SizeType i = 0; i < nodesArray.Size(); ++i) {
			rapidjson::Value& nodeObject = nodesArray[i];
			std::string name;
			std::string type;

			vector<std::string >ss;

			if (nodeObject.IsObject()) {
				if (nodeObject.HasMember("name") && nodeObject["name"].IsString()) {
					 name = nodeObject["name"].GetString();
					//std::cout << "Name: " << name << std::endl;
				}
				if (nodeObject.HasMember("type") && nodeObject["type"].IsString()) {
					type = nodeObject["type"].GetString();
					//std::cout << "Type: " << type << std::endl;
				}
				//std::cout << "Name: " << name << std::endl;
				//如果有值value 拿值 赋值
				Value& attributes = nodeObject["attributes"];
				if (attributes.IsArray()) {
					for (SizeType j = 0; j < attributes.Size(); ++j) {
						Value& attribute = attributes[j];

						if (attribute.HasMember("value")) {
							Value& value = attribute["value"];
							//赋值函数
							if (value.IsArray()) {
								for (SizeType k = 0; k < value.Size(); ++k) {
									if (value[k].IsString()) {
										ss.push_back(value[k].GetString());
									}
								}
							}
							else if (value.IsString()) {
								ss.push_back(value.GetString());
								//cout << value.GetString() << "!!!!!!!!!!!!!";
								
							}

						}
					}
				}
				
			}

			//每轮循环 生成一个node   ss传递的是参数	
			for (int t = 0; t < ss.size(); t++) cout << "ss" <<t<<" " << ss[t] << " \n";
			createNode(type, name, examplePass, ss);
			examplePass.check();
			printf("create success:"); std::cout << name<<"cccc time" << i << "\n";

		}

	}

	

	// 遍历数组并存储字符串元素
	if (document1.HasMember("links") && document1["links"].IsArray()) {
		const rapidjson::Value& linksArray = document1["links"];
		for (rapidjson::SizeType i = 0; i < linksArray.Size(); ++i) {
			const rapidjson::Value& linkObject = linksArray[i];
			std::string outNode;
			std::string outPort;
			std::string inNode;
			std::string inPort;
			if (linkObject.IsObject()) {
				if (linkObject.HasMember("outNode") && linkObject["outNode"].IsString()) {
					outNode = linkObject["outNode"].GetString();
					//std::cout << "outNode: " << outNode << std::endl;
				}
				if (linkObject.HasMember("outPort") && linkObject["outPort"].IsString()) {
					outPort = linkObject["outPort"].GetString();
					//std::cout << "outPort: " << outPort << std::endl;
				}
				if (linkObject.HasMember("inNode") && linkObject["inNode"].IsString()) {
					inNode = linkObject["inNode"].GetString();
					//std::cout << "inNode: " << inNode << std::endl;
				}
				if (linkObject.HasMember("inPort") && linkObject["inPort"].IsString()) {
					inPort = linkObject["inPort"].GetString();
					//std::cout << "inPort: " << inPort << std::endl;
				}
			}
			//每轮循环 bind两个点
			bindNode(outNode, outPort, inNode, inPort ,examplePass);
			printf("bind success:"); std::cout << outNode  << inNode << i << "\n";
		}
	}






	// log path  要改
	std::string logFilePath = "C:\\Users\\w1625\\Desktop\\PhotoGraph\\log.txt";
	//log
	std::ofstream logfile(logFilePath, std::ios::trunc);  // trunc覆盖写  app追加
	if (logfile.is_open()) {
	//get time
	logfile << "[IsValidGraph]: ";
		if (examplePass.isValid()) 	logfile << "True" << std::endl;
		else logfile << "False" << std::endl;
		logfile.close();
		std::cout << "Log file written successfully." << std::endl;
	}
	else {
		std::cout << "Failed to open log file." << std::endl;
	}

	examplePass.sequenceGeneration();
	cout << "good" << endl;
	examplePass.work();

	//printf("%d\n", clock() - start_time);

	//图片文件要写出

	cv::imshow("Example Output", examplePass.getTexture()->toMat());
	cv::waitKey(0);

	return 0;
}
