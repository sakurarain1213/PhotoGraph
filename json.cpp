
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <string>
#include <iostream>
#include <vector>

using namespace rapidjson;
using namespace std;

int tmain() {


	/*   name  +
	* type  写void if 生成create
	* attribute   只要name和value（arr或者string） 类里写json解析方法   
	/*
	Node_SaltAndPepperNoise *temp=examplePass.getNode<Node_SaltAndPepperNoise>("N");
	temp用每个类的pulic方法去访问

	* link  去bind循环
*/	


	std::string jsonStr1 = R"({"name":"未命名","nodes":[{"name":"节点0","type":"Texture","position":{"x":261,"y":281},"attributes":[{"name":"Texture","type":"Tex","value":"C:\\Users\\Lierick\\Downloads\\logo.png"}],"inPorts":[],"outPorts":[{"name":"Tex","type":"Tex","isIn":false}]},{"name":"节点1","type":"Sample Texture","position":{"x":610,"y":371},"attributes":[],"inPorts":[{"name":"Tex","type":"Tex","isIn":true},{"name":"UV","type":"2","isIn":true}],"outPorts":[{"name":"Out","type":"4","isIn":false}]},{"name":"节点2","type":"Output","position":{"x":932,"y":351},"attributes":[],"inPorts":[{"name":"In","type":"4","isIn":true}],"outPorts":[]}],"links":[{"outNode":"节点0","outPort":"Tex","inNode":"节点1","inPort":"Tex"},{"outNode":"节点1","outPort":"Out","inNode":"节点2","inPort":"In"}]})";
	
	std::string jsonStr = R"({"name":"未命名","nodes":[{"name":"节点0","type":"Texture","position":{"x":281,"y":312},"attributes":[{"name":"Texture","type":"Tex","value":""}],"inPorts":[],"outPorts":[{"name":"Tex","type":"Tex","isIn":false}]},{"name":"节点1","type":"Matrix3","position":{"x":292,"y":487},"attributes":[{"name":"Mat","type":"3x3","value":"aksdjlasjdlawjdilawjdalwjdaowj"}],"inPorts":[],"outPorts":[{"name":"Out","type":"3x3","isIn":false}]},{"name":"节点2","type":"Matrix Sample","position":{"x":608,"y":382},"attributes":[{"name":"default_mat","type":"3x3","value":["0","0","0","0","0","0","0","0","0"]}],"inPorts":[{"name":"Tex","type":"Tex","isIn":true},{"name":"UV","type":"2","isIn":true},{"name":"Mat","type":"3x3","isIn":true}],"outPorts":[{"name":"Out","type":"4","isIn":false}]},{"name":"节点3","type":"Output","position":{"x":956,"y":382},"attributes":[],"inPorts":[{"name":"In","type":"4","isIn":true}],"outPorts":[]}],"links":[{"outNode":"节点0","outPort":"Tex","inNode":"节点2","inPort":"Tex"},{"outNode":"节点1","outPort":"Out","inNode":"节点2","inPort":"Mat"},{"outNode":"节点2","outPort":"Out","inNode":"节点3","inPort":"In"}]})";
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
			if (nodeObject.IsObject()) {
				 Value& attributes = nodeObject["attributes"];

				if (attributes.IsArray()) {
					for (SizeType j = 0; j < attributes.Size(); ++j) {
						 Value& attribute = attributes[j];
						if (attribute.HasMember("value")) {
							Value& value = attribute["value"];
							if (value.IsArray()) {
								for (SizeType k = 0; k < value.Size(); ++k) {
									if (value[k].IsString()) {
										std::cout << value[k].GetString() << std::endl;
									}
								}
							}
							else if (value.IsString()) {
								std::cout << value.GetString() << std::endl;
							}
						}
					}
				}
			}
		}
	}




	// 遍历数组并存储
	if (document1.HasMember("links") && document1["links"].IsArray()) {
		const rapidjson::Value& linksArray = document1["links"];
		for (rapidjson::SizeType i = 0; i < linksArray.Size(); ++i) {
			const rapidjson::Value& linkObject = linksArray[i];
			if (linkObject.IsObject()) {
				if (linkObject.HasMember("outNode") && linkObject["outNode"].IsString()) {
					std::string outNode = linkObject["outNode"].GetString();
					std::cout << "outNode: " << outNode << std::endl;
				}
				if (linkObject.HasMember("outPort") && linkObject["outPort"].IsString()) {
					std::string outPort = linkObject["outPort"].GetString();
					std::cout << "outPort: " << outPort << std::endl;
				}
				if (linkObject.HasMember("inNode") && linkObject["inNode"].IsString()) {
					std::string inNode = linkObject["inNode"].GetString();
					std::cout << "inNode: " << inNode << std::endl;
				}
				if (linkObject.HasMember("inPort") && linkObject["inPort"].IsString()) {
					std::string inPort = linkObject["inPort"].GetString();
					std::cout << "inPort: " << inPort << std::endl;
				}
			}




		}


	}
	return 0;
}

