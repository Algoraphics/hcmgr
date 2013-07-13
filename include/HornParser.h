// HornParser.h
//
// This file contains the definition for the HornParser class, which is in charge of 
// parsing the horn clauses.

//#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <istream>
#include <iterator>
#include <fstream>
#include "SymbolTable.h"

using namespace std;

class HornParser {
private:
	SymbolTable the_table;
public:
	vector<string> tokenize(string s, char token);
	vector<string> splitAtChar(string s, char token);
	string process(string filename);
	bool isHorn(string s, Node & hc_node);
	bool isBody(string s, Node & body_node);
	bool isPred(string s, Node & pred_node);
	bool isSymbol(string s, Node & sym_node);
	bool isLabel(string s);
	bool isNumber(string s);
	SymbolTable parseHornFile(string filename);
	string trim(string s);
};