#pragma once

#ifndef _INPUTPORT_H
#define _INPUTPORT_H

#include <map>
#include <string>

template <typename T> class OutputPort {
private:
	T value;
public:
	OutputPort() { 
		value = T();
	}
	inline void setValue(T value) {
		this->value = value;
	}
	inline T getValue() {
		return this->value;
	}
};

template <typename T> class InputPort {
private:
	OutputPort<T>* output;
public:
	InputPort() {
		output = NULL; 
	}
	inline bool isBinded() { 
		return output != NULL; 
	}
	inline void bind(OutputPort<T>* port) {
		this->output = port;
	}
	inline T getValue() {
		return this->output->getValue();
	}
};

class InputPortMap {
private:
	std::map<std::string, size_t> ip_map_;
public:
	template <typename T>
	InputPort<T>* getInputPort(std::string port_name);
	template <typename T>
	T getInput(std::string port_name);
	template <typename T>
	InputPort<T>* defineInputPort(std::string port_name);
};

class OutputPortMap {
private:
	std::map<std::string, size_t> op_map_;
public:
	template <typename T>
	OutputPort<T>* getOutputPort(std::string port_name);
	template <typename T>
	void setOutput(std::string port_name, T value);
	template <typename T>
	OutputPort<T>* defineOutputPort(std::string port_name);
};

template <typename T>
InputPort<T>* InputPortMap::getInputPort(std::string port_name) {
	if (ip_map_.find(port_name) != ip_map_.end())
		return (InputPort<T>*)ip_map_[port_name];
	else return NULL;
}

template <typename T>
T InputPortMap::getInput(std::string port_name) {
	if (ip_map_.find(port_name) != ip_map_.end())
		return ((InputPort<T>*)ip_map_[port_name])->getValue();
	else return T();
}

template <typename T>
InputPort<T>* InputPortMap::defineInputPort(std::string port_name) {
	InputPort<T>* obj = new InputPort<T>();
	ip_map_[port_name] = (size_t)(obj);
	return obj;
}

template <typename T>
OutputPort<T>* OutputPortMap::getOutputPort(std::string port_name) {
	if (op_map_.find(port_name) != op_map_.end())
		return (OutputPort<T>*)op_map_[port_name];
	else return NULL;
}

template <typename T>
void OutputPortMap::setOutput(std::string port_name, T value) {
	if (op_map_.find(port_name) != op_map_.end())
		((OutputPort<T>*)op_map_[port_name])->setValue(value);
}

template <typename T>
OutputPort<T>* OutputPortMap::defineOutputPort(std::string port_name) {
	OutputPort<T>* obj = new OutputPort<T>();
	op_map_[port_name] = (size_t)(obj);
	return obj;
}

#endif
