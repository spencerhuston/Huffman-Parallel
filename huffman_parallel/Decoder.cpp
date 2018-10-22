#include <iostream>
#include <bitset>
#include <fstream>
#include <cmath>
#include "TreeNode.h"
#include "MinHeap.h"
#include "HuffTree.h"
#include "Decoder.h"

Decoder::Decoder(string huff_file_path)
{ 	
	huff_path = huff_file_path; 
	for (int i = 0; i < 256; i++)
		frequency_table[i] = 0;
}

//DO NOT CHANGE
void Decoder::buildFrequencyTableFromFile()
{
	//check opening mode
	FILE * input_file = fopen((huff_path).c_str(), "rb");
	
	//get file size for char buffer
	fseek(input_file, 0, SEEK_END);
	long int file_size = ftell(input_file);
	fclose(input_file);

	//make char buffer and read bytes
	input_file = fopen((huff_path).c_str(), "r+");
	unsigned char * buffer = (unsigned char *) malloc(file_size);
	int total_read = fread(buffer, sizeof(unsigned char), file_size, input_file);
	fclose(input_file);
	
	//do not change
	//convert bytes to char's and int's
	unique_chars = (buffer[1] << 8) | buffer[0];
	
	text_data_size = file_size - (2 + (unique_chars * 5));
	text_data = new unsigned char[text_data_size];

	for (int i = 0; i < text_data_size; i++)
	{ text_data[i] = buffer[i + (2 + unique_chars * 5)]; }
 
	vector<int> characters;
	vector<int> frequencies;
	for (int i = 2; i < unique_chars * 5; i += 5)
	{
		characters.push_back(buffer[i]);
		int freq = (buffer[i + 4] << 24) | (buffer[i + 3] << 16) | (buffer[i + 2] << 8) | (buffer[i + 1]);
		frequencies.push_back(freq);
	}

	//move data to frequency_table
	for (int i = 0; i < characters.size(); i++)
	{ frequency_table[abs((int)characters[i])] = frequencies[i]; }
	
	free(buffer);
}

void Decoder::decode(string output_file_path)
{
	MinHeap * mh = new MinHeap();
	for (int i = 0; i < 256; i++)
		if (frequency_table[i] != 0)
		{
			TreeNode * new_node = new TreeNode(i, frequency_table[i]);
			mh->insert(new_node);
		}
	HuffTree * huff = new HuffTree();
	huff->buildTree(mh);
	delete mh;
	writeUncompressedFile(output_file_path, huff);
	delete huff;
}

void Decoder::writeUncompressedFile(string file_path, HuffTree * huff)
{
	string bit_string;
	for (int i = 0; i < text_data_size; i++)
	{ bit_string.append((bitset<8>(text_data[i])).to_string()); }	
				
	int total_chars = 0;
	for (int i = 0; i < 256; i++)
		if (frequency_table[i] != 0)
			total_chars++;

//	huff->printCodes();
		
	int *times_used = new int[256];	
	for (int i = 0; i < 256; i++) { times_used[i] = 0; }
	string text_body;
	TreeNode * node = huff->getRoot();
	for (int i = 0; i < bit_string.length(); i++)
	{
		if ((bit_string[i] == '0') == true)
			if (node->getLeft())
				node = node->getLeft();	
		if ((bit_string[i] == '1') == true)
			if (node->getRight())
				node = node->getRight();

		if (node->isLeafNode())
		{
			string val;
			val  = ((char)node->getVal());
			times_used[node->getVal()]++;
			
			if (times_used[(unsigned)node->getVal()] > frequency_table[(unsigned)node->getVal()])
				break;
			else
				text_body += val;
			node = huff->getRoot();
		}		
	}
	
	ofstream output_file(file_path);
	output_file << text_body;
	output_file.close();
	delete[] times_used;
}

Decoder::~Decoder()
{ delete[] text_data; }
