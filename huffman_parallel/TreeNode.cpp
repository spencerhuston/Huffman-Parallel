#include <iostream>
#include "TreeNode.h"
#include <string>

TreeNode::TreeNode(unsigned char value, unsigned int freq)
{
	val = value;
	frequency = freq;
	left = NULL;
	right = NULL;
}

bool TreeNode::isLeafNode()
{ return (left == NULL && right == NULL) ? true : false; }

TreeNode * TreeNode::getLeft()
{ return left; }

TreeNode * TreeNode::getRight()
{ return right; }

void TreeNode::setLeft(TreeNode * node)
{ left = node; }

void TreeNode::setRight(TreeNode * node)
{ right = node; }

void TreeNode::join(TreeNode * left_child, TreeNode * right_child)
{
	left = left_child;
	right = right_child;
}

unsigned TreeNode::getFrequency()
{ return frequency; }

unsigned TreeNode::getVal()
{ return val; }

TreeNode::~TreeNode() 
{ 
	left = NULL;
	right = NULL;
	val = 0;
	frequency = 0;
}
