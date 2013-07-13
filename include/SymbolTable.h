// SymbolTable.h
//
// Contains definitions for our SymbolTable class as well as our SymbolTableEntry class.
// The SymbolTableEntry class is then subclassed into Predicate, Variable, and Constant
// classes so that SymbolTable only needs one vector of entries.

//#include "stdafx.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include "NodesLibrary.h"

using namespace std;




class SymbolTableEntry {
protected:
        string name;
public:
        SymbolTableEntry(string name);
        virtual string getEntry() = 0;
};

class Predicate: public SymbolTableEntry {
private:
        vector<shared_ptr<SymbolTableEntry>> args;      
public:
        Predicate(string name);
        virtual string getEntry();
        vector<shared_ptr<SymbolTableEntry>> getArgs();
        void addArg(shared_ptr<SymbolTableEntry>);
};


class Variable: public SymbolTableEntry {
public:
        Variable(string name);
        virtual string getEntry();
};


class Constant: public SymbolTableEntry {
private:
        int value;
public:
        Constant(string name);
        virtual string getEntry();
};

class SymbolTable {
private:
        vector<shared_ptr<SymbolTableEntry>> symbols;
public:
        void insertPredicate(Node);
        shared_ptr<SymbolTableEntry> containsSymbol(shared_ptr<SymbolTableEntry>);
        void print();
        void unifyAllPred();
};


bool unifyPair(Predicate*, Predicate*);