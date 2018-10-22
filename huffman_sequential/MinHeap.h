#ifndef MINHEAP_H
#define MINHEAP_H

#include "TreeNode.h"
#include <vector>

using namespace std;

class MinHeap
{
	private:
		vector<TreeNode *> heap;
		int size;
		TreeNode * null_root; //empty node at 0th index	
	
	public:
		MinHeap();
		TreeNode * removeMin(); //returns root of heap
		void insert(TreeNode * val); //adds element to heap
		int getSize(); //returns size of heap
		int getParent(int index);
		int getLeft(int index);
		int getRight(int index);
		void down_heap(int index);
		void swap_nodes(TreeNode &curr, TreeNode &other);
		void print_contents();
		~MinHeap();
};

#endif
