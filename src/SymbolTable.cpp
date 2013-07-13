// SymbolTable.cpp
//
// Contains implementation for our SymbolTable class as well as our SymbolTableEntry class.
// The SymbolTableEntry class is then subclassed into Predicate, Variable, and Constant
// classes so that SymbolTable only needs one vector of entries.

//#include "stdafx.h"
#include "../include/SymbolTable.h"
#include <map>

using std::cout;
using namespace std;


SymbolTableEntry::SymbolTableEntry(string _name)
        :name(_name) {}

Predicate::Predicate(string _name) 
        :SymbolTableEntry(_name) {}

string Predicate::getEntry(){
        return name;
}

void Predicate::addArg(shared_ptr<SymbolTableEntry> ste){
        args.push_back(ste);
}

vector<shared_ptr<SymbolTableEntry>> Predicate::getArgs(){
        return args;
};

Variable::Variable(string _name)
        :SymbolTableEntry(_name) {}

string Variable::getEntry(){
        return name;
}

Constant::Constant(string _name)
        :SymbolTableEntry(_name) {}

string Constant::getEntry(){
        return name;
}

void SymbolTable::insertPredicate(Node n){
        string nameStr = n.get_data();
        vector<shared_ptr<Node>> v = n.get_leaves();
        shared_ptr<Predicate> p(new Predicate(nameStr));

        for(vector<shared_ptr<Node>>::iterator it = v.begin(); it!= v.end(); it++){
                //so this is us getting the actual leaf node that is either a label or a number
                shared_ptr<Node> tempNode = (*it)->get_leaves()[0];


                if(tempNode->get_id() == label){
                        shared_ptr<SymbolTableEntry> tempSTE(new Variable(tempNode->get_data()));

                        if(containsSymbol(tempSTE) == 0){
                                symbols.push_back(tempSTE);
                        }
						else
						{
							cout << tempSTE->getEntry() + " is already in the table!" << endl;
						}
                        
                        p->addArg(containsSymbol(tempSTE));
                }else if(tempNode->get_id() == number){
                        Constant con(tempNode->get_data());
                        shared_ptr<SymbolTableEntry> tempSTE(new Constant(tempNode->get_data()));

                        if(containsSymbol(tempSTE) == 0){
                                symbols.push_back(tempSTE);
                        }
						else
						{
							cout << tempSTE->getEntry() + " is already in the table!" << endl;
						}

                        p->addArg(containsSymbol(tempSTE));
                }       
        }

        shared_ptr<SymbolTableEntry> tempSTE(p);

        if(containsSymbol(tempSTE) == 0){
                symbols.push_back(tempSTE);
        }
		else
		{
			cout << tempSTE->getEntry() + " is already in the table!" << endl;
		}
}

shared_ptr<SymbolTableEntry> SymbolTable::containsSymbol(shared_ptr<SymbolTableEntry> ste){

        for(unsigned int i = 0; i<symbols.size(); i++){
                shared_ptr<SymbolTableEntry> tempSym = symbols[i];

                if(tempSym->getEntry() == ste->getEntry()){
                        return symbols[i];
                }
        }

        return 0;
}

void SymbolTable::print() {
        for(unsigned int i=0; i<symbols.size(); ++i) {
                if(dynamic_cast< Predicate *> (symbols[i].get()) != NULL){
                        cout << symbols[i]->getEntry() << endl;
                }
        }
}




bool unifyPair(Predicate* p1, Predicate* p2){
        //subs is a map of substitutions represented by (key, value) pairs, where the
        // Key is a label which should be substituted by the label or const of the value.

        map<shared_ptr<SymbolTableEntry>,shared_ptr<SymbolTableEntry>> subs;

        vector<shared_ptr<SymbolTableEntry>> args1 = p1->getArgs();
        vector<shared_ptr<SymbolTableEntry>> args2 = p2->getArgs();

        //check if the number of args is the same
        if(args1.size() != args2.size()){
                
                cout<< "the two predicates do not have the same number of arguments" << endl;
                return false;
        
        }

        //check if the predicate names are the same
        if(args1[0]->getEntry() != args2[0]->getEntry()){
                
                return false;
        }

        string resultP1 = "(";
        resultP1.append(args1[0]->getEntry());
        resultP1.append(" ");

        string resultP2 = resultP1;

        for(unsigned int i = 1;i<args1.size();i++){

                        //grab the two corresponding args
                        shared_ptr<SymbolTableEntry> sym1 = args1[i];
                        shared_ptr<SymbolTableEntry> sym2 = args2[i];

                        //depending on the arg types, take appropriate action
                        if(dynamic_cast< Constant *> (sym1.get()) != NULL && dynamic_cast< Constant *> (sym1.get()) != NULL){
                                
                                //if they are both constants, check their values and if they don't match return false                           
                                if(sym1->getEntry() != sym2->getEntry()){
                                        return false;
                                }

                        }else{
                                //otherwise, first make the needed substitutions for sym1 and sym2
                                if(dynamic_cast< Variable *> (sym1.get()) != NULL){
                                        
                                        while(subs.find(sym1) !=subs.end()){
                                                sym1 = subs[sym1];
                                        }

                                }

                                if(dynamic_cast< Variable *> (sym2.get()) != NULL){
                                        
                                        while(subs.find(sym2) !=subs.end()){
                                                sym2 = subs[sym2];
                                        }

                                }

                                //now if they are both constants check if they are equal
                                if(dynamic_cast< Constant *> (sym1.get()) != NULL && dynamic_cast< Constant *> (sym1.get()) != NULL){
                                        
                                        //if they don't match return false
                                        if(sym1->getEntry() != sym2->getEntry()){
                                                return false;
                                        }

                                }else if(sym1->getEntry() == sym2->getEntry()){
                                        //otherwise, if they are both equal then they must both be the same variable.
                                        //so, do nothing
                                
                                }else if(dynamic_cast< Variable *> (sym2.get()) != NULL){

                                        //otherwise, if sym2 is a variable, it should be substituted by sym1 always
                                        //add an entry to the substitution map
                                        subs[sym2] = sym1;

                                }else if(dynamic_cast< Variable *> (sym1.get()) != NULL){
                                        
                                        //otherwise, if sym2 is not a variable it must a be a constant. If sym1 is a variable
                                        //if should be substituted by sym2. add an entry to the substitution map
                                        subs[sym1] = sym2;
                                
                                }
                        
                        }

                        resultP1.append(sym1->getEntry() + " ");
                        resultP2.append(sym2->getEntry()+ " ");
        }

        resultP1 = resultP1.substr(0,resultP1.length()-1) + ")";
        resultP2 = resultP2.substr(0,resultP2.length()-1) + ")";

        cout << "We have a match: " << endl;
        cout << "       P1: " << p1->getEntry() << endl;
        cout << "       P2: " << p2->getEntry() <<endl;

        if(subs.size() > 0){
                cout<< "The following substitutions exist: " << endl;
                for(map<shared_ptr<SymbolTableEntry>,shared_ptr<SymbolTableEntry>>::iterator iter = subs.begin(); iter != subs.end(); iter ++){
                        cout<< "        " << iter->second->getEntry() << " --> " << iter->first->getEntry() << endl;            
                }
        }else{
                cout<< "No substitutions were made." << endl;
		}


        //the resulting 
        cout<< "The transformed predicates:" << endl;
        cout << "       P1: " << resultP1 << endl;
        cout << "       P2: " << resultP2 <<endl;
        
		string unifiedPred = "(" + args1[0]->getEntry() + " ";

		for(unsigned int i = 1;i<args1.size();i++){
			shared_ptr<SymbolTableEntry> sym1 = args1[i];

			if(dynamic_cast< Variable *> (sym1.get()) != NULL){
                                        
                    while(subs.find(sym1) !=subs.end()){
                            sym1 = subs[sym1];
                    }
            }
			unifiedPred.append(sym1->getEntry()+" ");
		
		}
		unifiedPred = unifiedPred.substr(0,resultP2.length()-1) + ")";

		cout<< "The unified predicate: " << endl;
		cout<< "       " << unifiedPred << endl;

        cout << endl << endl;
        
        return true;
}


void SymbolTable::unifyAllPred(){
        //this should iterate through all predicate entries in the symbol table and check for matches.
        //if it finds any it should print out the appropriate information

        vector<Predicate*>  preds;

        //create a vector of all the predicates
        for(unsigned int i = 0; i<symbols.size();i++){
                if(dynamic_cast< Predicate *> (symbols[i].get()) != NULL){
                        preds.push_back(dynamic_cast< Predicate *> (symbols[i].get()));
                }
        }

        //if we have more than 1 predicate, then check each pair of predicates for matches
        if(preds.size() > 1){

                for(unsigned int i = 0; i<preds.size()-1; i++){
                        for(unsigned int j = i+1; j<preds.size(); j++){
                                
                                unifyPair(preds[i],preds[j]);

                        }
                }

        }


}
