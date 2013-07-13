//HornParser.cpp
//
// This file contains the implementation for the HornParser class, which is in charge of 
// parsing the horn clauses.


//#include "stdafx.h"
#include "../include/HornParser.h"

// This function opens the file containing horn clauses, iterates through all of the horn clauses,
// parses it down to the horn clause level, and, if valid, inserts the data from the horn clause 
// into the symbol table
SymbolTable HornParser::parseHornFile(string filename)
{
	vector<string> hornClauses;

	// pre-process the file into a readable format
	string procFilename = process(filename);

	// open pre-processed file and begin parsing
	ifstream file(procFilename);
	if (file.is_open())
	{
		string line;
		string clause = "";
		while (file.good())
		{
			 getline(file,line);
			 clause = clause + " " + line;
			 clause = trim(clause);

			 // if there is a blank line, skip it
			 if (clause.size() == 0) {
				 continue;
			 } else {
				 int parenCnt = 0;

				 // valid horn clause should begin/end with a parenthesis
				 if (clause[0] == '(')
				 {
					 ++parenCnt;
					 for(unsigned int i = 1; i < clause.length(); ++i)
					 {
						 if(clause[i] == '(')
						 {
							 ++parenCnt;
						 }
						 else if (clause[i] == ')')
						 {
							 --parenCnt;
							 if (parenCnt == 0)
							 {
								 // horn clause parsing ended, check if valid, create root node for tree
								 string checkClause = clause.substr(0,i + 1);
								 shared_ptr<Node> hc_node(new Node("",horn_clause));
								 if(isHorn(checkClause,*hc_node))
								 {
									 // add horn clause to valid list, add node data to symbol table
									 hornClauses.push_back(checkClause);
									 vector<shared_ptr<Node>> head_body = hc_node->get_leaves();
									 for (unsigned int j = 0; j < head_body.size(); ++j)
									 {
										 vector<shared_ptr<Node>> leaves = head_body[j]->get_leaves();
										 for (unsigned int k = 0; k < leaves.size(); ++k)
										 {
											 if (leaves[k]->get_id() == predicate)
											 {
												the_table.insertPredicate(*leaves[k]);									 
											 }
										 }
									 }
									 // keep track of the rest of the string if the horn clause ended before the line did
									 if (i + 1 < clause.length())
									 {
										clause = trim(clause.substr(i + 1,clause.length()));
										break;
									 }
									 else
									 {
										 clause = "";
										 break;
									 }
								 }
								 else 
								 {
									clause = "";
									break;
								 }
							 }
						 }
					 }
				 }
				 else
				 {
					 clause = "";
				 }
			 }
		}
		file.close();
	}
	// couldn't open file
	else {
		cout << "File could not be opened for reading." << endl << endl;
		string file_error = "file_does_not_exist";
		throw file_error;
	}
	the_table.print();
	return the_table;
}


// Pre-processor for the file. Will take all whitespace in the file and reduce it to a single whitespace
// Ex: "      " becomes " ". It also strips empty lines from the file.
string HornParser::process(string filename)
{
	ifstream file(filename);
	string procFilename = filename.substr(0,filename.length() - 4) + "-process.txt";
	ofstream outFile(procFilename);
	if (file.is_open())
	{
		string line;
		while (file.good())
		{
			getline(file,line);
			if (line != "")
			{
				vector<string> tokens = tokenize(line,' ');
				vector<string> outTokens;
				for (unsigned int i = 0; i < tokens.size(); ++i)
				{
					if(tokens[i] != "")
					{
						outTokens.push_back(tokens[i]);
					}
				}
				string outLine = "";
				for (int j = 0; j < ((int)outTokens.size()) - 1; ++j)
				{
					outLine += outTokens[j] + " ";
				}
				outLine += outTokens[outTokens.size() - 1] + "\n";
				outFile << outLine;
			}
		}
	} else {
		cout << "File " << filename << " could not be opened" << endl << endl;
		string file_error = "file_does_not_exist";
		throw file_error;
	}
	return procFilename;
}

bool HornParser::isHorn(string s, Node & hc_node)
{
	if (s[0] == '(' && s[s.length() - 1] == ')')
	{
		string preds = s.substr(1,s.length() - 2); //cut off the end parens
		int index = preds.find(')');
		if (index == -1) return false; //A trailing paren wasn't found
		string headStr = trim(preds.substr(0,index + 1));
		if (headStr[0] == '(' && headStr[headStr.length() - 1] == ')')
		{//This block pulls excess whitespace from the inside of the string
			headStr = "(" + trim(headStr.substr(1,headStr.size() - 2)) + ")";
		} else return false;
		shared_ptr<Node> head_node(new Node(headStr,head));
		shared_ptr<Node> pred_node(new Node(headStr,predicate));

		if (isPred(headStr,*pred_node))
		{
			head_node->add_node(pred_node);
			hc_node.add_node(head_node);
			
			
			if (index + 1 == preds.length())
			{//if there is no body
				return true;
			}
			else
			{
				string bodyStr = trim(preds.substr(index + 2, preds.length()));
				shared_ptr<Node> body_node(new Node(bodyStr,body));
				if (isBody(bodyStr,*body_node))
				{
					hc_node.add_node(body_node);
					return true;
				}
			}
		}
	}
	return false;
}

bool HornParser::isBody(string s, Node & body_node)
{
	if (s[0] == '(' && s[s.length() - 1] == ')')
	{
		string preds = trim(s.substr(1,s.length() - 2)); //cut off the end parens
		int index = preds.find(')'); //find first closing paren(end of first predicate)
		bool is = true;
		if (index == -1) return false;
		while(index != preds.length() - 1) //stop looping through predicates when the end of the next one is the end of the string
		{
			string predStr = trim(preds.substr(0,index + 1));
			if (predStr[0] == '(' && predStr[predStr.length() - 1] == ')')
			{
				predStr = "(" + trim(predStr.substr(1,predStr.size() - 2)) + ")";
			} else return false;
			shared_ptr<Node> pred_node(new Node(predStr,predicate));
			if(isPred(predStr,*pred_node)) //check if the first predicate is valid
			{
				body_node.add_node(pred_node);
				if (preds[index + 1] == ' ' && preds[index + 2] == '(')
				{
					preds = preds.substr(index + 2, preds.length()); //cut the first predicate and leading space off the string
					index = preds.find(')'); //move index to the next ending paren
				}
				else return false;
			}
			else
			{
				is = false;
				break;
			}
		}
		string predStr = trim(preds.substr(0,index + 1));
		if (predStr[0] == '(' && predStr[predStr.length() - 1] == ')')
		{
			predStr = "(" + trim(predStr.substr(1,predStr.size() - 2)) + ")";
		} else return false;
		shared_ptr<Node> pred_node(new Node(predStr,predicate));
		if (is && isPred(preds,*pred_node)) //check that we have all valids so far, then make sure the last predicate is valid.
		{
			body_node.add_node(pred_node);
			return true;
		}
	}
	return false;
}

bool HornParser::isPred(string s, Node & pred_node)
{
	vector<string> tokens = tokenize(s,' ');
	string nameStr = tokens[0];
	string lastSym = tokens[tokens.size() - 1];
	char lastChar = lastSym[lastSym.size() - 1];
	if (tokens.size() == 1) //There are no arguments
	{
		nameStr = trim(nameStr.substr(1,nameStr.length() - 2));
		shared_ptr<Node> name_node(new Node(nameStr,name));
		if(isLabel(nameStr))
		{
			pred_node.add_node(name_node);
			shared_ptr<Node> label_node(new Node(nameStr,label));
			name_node->add_node(label_node);
			return true;
		}
		return false;
	}
	//This next line pulls the close paren off the last arg so I can loop through the args without worrying about it
	tokens[tokens.size() - 1] = lastSym.substr(0,lastSym.size() - 1);
	if (nameStr[0] == '(' && lastChar == ')')
	{
		string nameSub = trim(nameStr.substr(1,nameStr.size()));
		shared_ptr<Node> name_node(new Node(nameSub,name));
		if (isLabel(nameSub))
		{
			pred_node.add_node(name_node);
			shared_ptr<Node> label_node(new Node(nameSub,label));
			name_node->add_node(label_node);
			bool is = true;
			for (unsigned int i = 1; i < tokens.size(); ++i) //loop over all the args, skip the first token, which is the name
			{
				string symStr = trim(tokens[i]);
				shared_ptr<Node> sym_node(new Node(symStr,symbol));
				if (isSymbol(symStr,*sym_node))
				{
					pred_node.add_node(sym_node);
				}
				else
				{
					is = false;
					break;
				}
			}
			return is;
		}
	}
	return false;
}

bool HornParser::isSymbol(string s, Node & sym_node) //use isalpha function for label checking
{
	if (isLabel(s))
	{
		shared_ptr<Node> label_node(new Node(s,label));
		sym_node.add_node(label_node);
	}
	else if (isNumber(s))
	{
		shared_ptr<Node> num_node(new Node(s,number));
		sym_node.add_node(num_node);
	}
	else
	{
		return false;
	}
	return true;
}

bool HornParser::isLabel(string s)
{
	bool is = true;
	for (unsigned int i = 0; i < s.length(); ++i)
	{
		if (!isalpha(s[i]))
		{
			  is = false;
			  break;
		}
	}
	return is;
}

bool HornParser::isNumber(string s)
{
	bool is = true;
	for (unsigned int i = 0; i < s.length(); ++i)
	{
		if (!isdigit(s[i]))
		{
			  is = false;
			  break;
		}
	}
	return is;
}

//This function takes in a string s and returns a vector of strings separated by a char token
vector<string> HornParser::tokenize(string s, char token)
{
	stringstream stream(s);
	vector<string> results;
	while (!stream.eof())
	{
		string line;
		getline(stream,line,token);
		results.push_back(line);
	}
	return results;
}

//This function takes a string s and splits it into two strings separated by the first instance of a char token
vector<string> HornParser::splitAtChar(string s, char token)
{
	int index = s.find(token);
	vector<string> results;
	if (index == string::npos) //npos is a string member variable that means this function failed
	{
		index = s.length();
	}
	results.push_back(s.substr(0,index));
	results.push_back(s.substr(index + 1,s.length()));
	return results;
}

//This function pulls all whitespace off the beginning and end of a string
string HornParser::trim(string s)
{
	string whitespaces(" \t\f\v\n\r");
	string result;
	int begindex = s.find_first_not_of(whitespaces);
	if (begindex != -1) {
		result = s.erase(s.find_last_not_of(whitespaces) + 1);
		return result.substr(begindex,result.length());
	}
	return "";
}
