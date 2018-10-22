#include <iostream>
#include <climits>
#include "MinHeap.h"

//implement the methods in MinHeap.h

MinHeap::MinHeap()
{
	size = 0;
	//dummy node
	heap.push_back(null_root);	
}

TreeNode * MinHeap::removeMin()
{
	if (size == 0)
		return NULL;
	if (size == 1)
	{
		size--;
		TreeNode * return_val = heap[1];
		heap.erase(heap.begin() + 1);
		return return_val;
	}
	
	//remove root and swap with last node
	TreeNode * root = heap[1];
	heap[1] = heap[size];
	heap.erase(heap.begin() + size);
	size--;
	
	down_heap(1);
	return root;
}

void MinHeap::insert(TreeNode * val)
{
	heap.push_back(val);
	size = heap.size() - 1;
	int index = size;

	//upheap if applicable	
	while (index != 1 && heap[getParent(index)]->getFrequency() > heap[index]->getFrequency())
	{
			swap(heap[index], heap[getParent(index)]);
			index = getParent(index);
	}
}

int MinHeap::getSize()
{ return size; }

int MinHeap::getParent(int index)
{ return (index/2); }

int MinHeap::getLeft(int index)
{ return (index*2); }

int MinHeap::getRight(int index)
{ return (index*2 + 1); }

void MinHeap::down_heap(int index)
{
	TreeNode * min = heap[index];
	
	int min_index = 0;
	if (getLeft(index) <= size && heap[getLeft(index)]->getFrequency() < heap[index]->getFrequency())
	{ 
		min = heap[getLeft(index)]; 
		min_index = getLeft(index);
	}
	if (getRight(index) <= size && heap[getRight(index)]->getFrequency() < min->getFrequency())
	{
		min = heap[getRight(index)]; 
		min_index = getRight(index);
	}
	if (min != heap[index])
	{ 
		swap(heap[index], heap[min_index]);
		down_heap(min_index);
	}
}

void MinHeap::swap_nodes(TreeNode &curr, TreeNode &other)
{
	TreeNode temp = curr;
	curr = other;
	other = temp;
}

MinHeap::~MinHeap()
{ heap.erase(heap.begin()); }
