// NodesLibrary.cpp
//
// Contains the implementation of the nodes that make up the parse tree.

//#include "stdafx.h"
#include "../include/NodesLibrary.h"

using namespace std;

Node::Node(string _data, hc_token _id)
	:data(_data),id(_id){}

string Node::get_data() {
	return data;
}

hc_token Node::get_id() {
	return id;
}

void Node::add_node(shared_ptr<Node> new_node) {
	node_ptrs.push_back(new_node);
}

vector<shared_ptr<Node>> Node::get_leaves() {
	return node_ptrs;
}
