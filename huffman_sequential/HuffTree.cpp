#include <iostream>
#include "HuffTree.h"

HuffTree::HuffTree() : root(new TreeNode(0, 0)) { }

void HuffTree::buildTree(MinHeap * mh)
{
	//WORKS CORRECTLY DO NOT CHANGE
	while (mh->getSize() > 1)
	{
		TreeNode * child1 = mh->removeMin();
		TreeNode * child2 = mh->removeMin();
		TreeNode * current = new TreeNode('\0', child1->getFrequency() + child2->getFrequency());
		current->join(child2, child1);
		//join child1 and child2
		mh->insert(current);
	}
	delete root;
	root = NULL;
	root = mh->removeMin();
	generateCodes(root);
}

//WORKS DO NOT CHANGE
void HuffTree::generateCodes(TreeNode * root)
{
	//preorder traversal on HuffTree
	//assign created bit-string to ascii-valued index in bit-string array
	string bit_string;
	preorder_traversal(root, bit_string);
}

//WORKS DO NOT CHANGE
void HuffTree::preorder_traversal(TreeNode * node, string bit_string)
{
	//if current node is internal check if left exists
	//append 0 to string and make left child current node
	//then check if right exists and repeat
	if (node->isLeafNode() == false)
	{
		if (node->getLeft())
		{ preorder_traversal(node->getLeft(), bit_string + "0"); }
		if (node->getRight())
		{ preorder_traversal(node->getRight(), bit_string + "1"); }
	}
	
	//set ascii-valued index element equal to bit_string
	//remove last appended character from string
	else
	{ bit_codes[(unsigned)(node->getVal())] = bit_string; }
}

void HuffTree::printCodes()
{
	for (int i = 0; i < 256; i++)
		cout << i << ": " << getCharCode(i) << endl;
}

TreeNode * HuffTree::getRoot()
{ return root; }

string HuffTree::getCharCode(int c)
{ return bit_codes[c]; }

HuffTree::~HuffTree() 
{ delete_nodes(root); }

void HuffTree::delete_nodes(TreeNode * root)
{
	//fix this
	if (root->isLeafNode())
	{
		delete root;
		root = NULL;
	}
	else
	{
		delete_nodes(root->getLeft());
		//cout << "Left: " << root->getLeft() << endl;	
		delete_nodes(root->getRight());
		//cout << "Right: " << root->getRight() << endl;
		delete root;
		root = NULL;
	}
}
