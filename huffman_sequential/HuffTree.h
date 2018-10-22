#ifndef HUFFTREE_H
#define HUFFTREE_H

#include "MinHeap.h"
#include "TreeNode.h"
#include <string>

#define MAXCODELENGTH 256
#define BYTESIZE 8

using namespace std;

class HuffTree
{
	protected:
		//Add class members and methods
		TreeNode * root;
		string bit_codes[MAXCODELENGTH];	

	public:
		HuffTree();

		//build a hiffman  tree  give a minheap
		void buildTree(MinHeap * mh);

		//generate codes by traversing the huffman tree
		//and store them in an internal data structure (array of strings for example)
		void generateCodes(TreeNode * root);

		void preorder_traversal(TreeNode * node, string bit_string);
			
		void printCodes();		
		//returns huffman code from  the ascii code
		TreeNode * getRoot();
	
		string getCharCode(int c);

		~HuffTree();
	
		void delete_nodes(TreeNode * root);	
};


#endif
