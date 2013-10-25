Parsing and Matching Predicates for Horn Clauses

Authors:
Ethan Rabb
Alex Gu
Tommy Powers

===========================================================================================
Compilation and Run Instructions:
===========================================================================================

This code has been ported so that it can be compiled on Unix machines as well as Windows, but
it was designed for an academic project with no expectation of portability, and as such does
not provide certain safety methods (e.g. include guards) against being extended or moved onto
other systems. Do so at your own risk.

Windows: simply open your favorite IDE (This project was written and tested in Visual Studio 
2010), add in all the source and header files and build the application. If you are not
running it from command line, you may need to define the command line arguments before running
to make sure your text file is parsed.
Note: to compile in visual studio, you will need to uncomment the #include "stdafx.h" line at
the top of each file. Similar action may be necessary for other IDE's, but YMMV.


Unix: Using g++, navigate to the src directory
g++ -std=gnu++0x HornClauseLab.cpp HornParser.cpp NodesLibrary.cpp SymbolTable.cpp -o ../hcmgr
This will place the hcmgr executable in the main directory.

To run the program in command line, navigate to the main directory and enter:
./hcmgr [path to text file]


The horn.txt file is given as an example text file from which the program will gather and
interpret horn clauses. For this project, output can be quite verbose for files containing
many syntax errors, as horn.txt does. This is useful for alerting users to existing errors.

A file with the line "-process" in it should appear in the directory after running the 
application, this is the output of a preprocessor used to remove extra whitespace and other
syntax issues.

===========================================================================================
Files:
===========================================================================================

HornClauseLab.cpp:

	This is the file that contains the program's main function, which parses a file of
	horn clauses, adds unique predicates, variables, and constants to a symbol table,
	and unifies the predicates through substitution.

HornParser.h
	
	This file contains the definition for the HornParser class, which is in charge of 
	parsing the horn clauses.


HornParser.cpp
	
	This file contains the implementation for the HornParser class, which is in charge of 
	parsing the horn clauses.


NodesLibrary.h

	This file contains the definitions of the nodes that make up the parse tree.


NodesLibrary.cpp

	This file contains the implementation of the nodes that make up the parse tree.

SymbolTable.h
	
	Contains definitions for our SymbolTable class as well as our SymbolTableEntry class.
	The SymbolTableEntry class is then subclassed into Predicate, Variable, and Constant
	classes so that SymbolTable only needs one vector of entries.


SymbolTable.cpp

	Contains implementation for our SymbolTable class as well as our SymbolTableEntry class.
	The SymbolTableEntry class is then subclassed into Predicate, Variable, and Constant
	classes so that SymbolTable only needs one vector of entries.

===========================================================================================
Design:
===========================================================================================

The abstract representation of our parser is modeled well as a tree, where each node of the 
tree is corresponds to a terminal or non-terminal token of the specified grammar. This tree
of nodes contains the data that we add to the symbol table--the strings that represent the 
predicates, variables, and constants.

In our implementation of the parse tree and symbol table, we created a Node class that had 
the same hierarchy as the input grammar. At the top, we have a horn clause node which has 
pointers to a head node and a body node, which in turn have pointers to predicate nodes, 
etc.. Once our program has decided that it has a valid horn clause, we follow that nodes 
down to the predicate level and insert the predicates. Since the predicate nodes also 
contain the label and number nodes, which correspond to variables and constants in the 
symbol table respectively, we insert unique labels and numbers into the symbol table at the
same time as the predicates.

Within our symbol table, we created a class called SymbolTableEntry that we subclassed into 
a Predicate class, Variable class, and Constant class, so that the table only needed one 
data structure for all of the entries. When we insert an entry into the table, we 
dynamically resolve a Predicate/Variable/Constant object from a SymbolTableEntry object, 
initialize it with data from a given node, and insert it into the vector of 
SymbolTableEntries.

For unification, we walk through the symbol table and extract all the predicate entries 
into a vector. Then we iterate over the predicate vector and make all pair-wise comparisons 
of the predicates. For each pair, we keep track of the list of substitution as a map from 
SymbolTableEntry to SymbolTableEntry. The key symbol in the map is substituted by its 
corresponding value. For every symbol we come across, we check if there is a key for it and 
continue to substitute as long as there is a key value for the symbol. We had issues 
getting a reference to the vector of predicates in the symbol table, and were instead 
creating a new predicates with the same data. This made applying the substitutions difficult. 
We resolved the issue by directly referencing the shared pointers in the symbol table vector. 

We had other difficulties with memory management, specifically initializing shared 
pointers. The other big challenge was managing white-space in the input horn clause file. 
We decided that we should not restrict the user to only use white-space to delimit tokens,
but that made it difficult to make our output uniform.


===========================================================================================
Testing
===========================================================================================

There were four main areas for testing:

First, the parser. We gave it malformed inputs such as strings with non-alphabetic 
characters, parentheses errors, empty parentheses,a nd blank lines. WE also gave it a range 
of well-formed inputs that contained only a head, a head and body, predicates with and 
without arguments, extra spaces in all parts of a horn clause, a horn clause split up on two 
lines, a hornclause on the same line as the latter half of another horn clause, and 
multi-predicate bodies.

The next main part that we tested was the symbol table. We tested this by creating duplicate 
predicates and arguments in our horn clauses and predicates with and without arguments.

The third main area to test was unification. We created prdicates with differing numbers of 
arguments to make sure we only compare pairs with the same number of arguments. We also 
designed an array of predicates with arguments that had one of the same labels, shifted 
labels, and constants. Additionally, in order to ensure that the substituitions took place 
regardless of the order on which predicate in a pair was listed listed first, we produced 
the unification output for both predicates, which worked as it should have.

Lastly, we tested that the program worked from the command line correctly. We tested it 
with malformed inputs such as the wrong number of arguments, incorrect arguments and made 
sure the usage string was being displayed. We also tested to make sure our program handles 
incorrect file names by printing out the error and the program returns a non-zero value.

===========================================================================================
Extra-Credit
===========================================================================================

As stated above in the second paragraph of our design section, we check if the 
SymbolTableEntry unique before we add it. We do this by iterating through the symbol table 
and making sure the entry-to-be-added isn't already in there. We then print out a message 
to alert the user that a duplicate entry was not added.
