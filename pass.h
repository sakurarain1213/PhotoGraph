#pragma once

#ifndef _PASS_H
#define _PASS_H

#include "node.h"
#include "vector"
#include <exception>
#include <iostream>

class NoOutputNodeException : public std::logic_error {
public:
	NoOutputNodeException() : std::logic_error("Missing output node in the pass") {}
	virtual ~NoOutputNodeException() throw() {}
};

class Pass {
private:
	std::map<std::string, Node*> node_map_;
	std::vector<Node*> node_sequence_;
	Node_Output* output;
public:
	Vec2f uv0;
	Vec2i screenPosition;
	Pass() : output(NULL) {}
	template <class T>
	void defineNode(std::string node_name) {
		node_map_[node_name] = new T(this);
	}
	template <>
	void defineNode<Node_Output>(std::string node_name) {
		output = new Node_Output(this);
		node_map_[node_name] = output;
	}
	template <class T>
	T* getNode(std::string node_name) {
		if (node_map_.find(node_name) != node_map_.end())
			return node_map_[node_name];
		return NULL;
	}
	void bind(std::string output_node, std::string output_port, std::string input_node, std::string input_port) {
		Node* opn = getNode<Node>(output_node);
		Node* ipn = getNode<Node>(input_node);
		if (opn != NULL && ipn != NULL) opn->bind(output_port, ipn, input_port);
	}
	void sequenceGeneration() {
		std::map<std::string, Node*>::iterator it;
		std::set<Node*>::iterator it2;
		for (it = node_map_.begin(); it != node_map_.end(); it++) {
			if ((*it).second->dependency_set.size() == 0) {
				node_sequence_.push_back((*it).second);
			}
		}
		for (int i = 0; i < node_sequence_.size(); ++i) {
			Node* nNode = node_sequence_[i];
			for (it2 = nNode->binded_set.begin(); it2 != nNode->binded_set.end(); it2++) {
				(*it2)->dependency_set.erase(nNode);
				if ((*it2)->dependency_set.size() == 0) {
					node_sequence_.push_back((*it2));
				}
			}
		}
	}
	void work() throw(NoOutputNodeException){
		if (output == NULL) throw NoOutputNodeException();
		for (int x = 0; x < output->width; ++x)
			for (int y = 0; y < output->height; ++y) {
				uv0 = Vec2f((x + 0.5) / output->width, (y + 0.5) / output->height);
				screenPosition = Vec2i(x, y);
				for (int i = 0; i < node_sequence_.size(); ++i) {
					node_sequence_[i]->work();
				}
			}
	}
};
#endif