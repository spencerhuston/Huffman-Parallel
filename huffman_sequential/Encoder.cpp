#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include <chrono>
#include "Encoder.h"

Encoder::Encoder(string input_file_path)
{ in_path = input_file_path; }

void Encoder::buildFrequencyTable()
{
	FILE * input_file = fopen((in_path).c_str(), "r");
	
	int c;
	while ((c = fgetc(input_file)) != EOF)
	{ frequency_table[c]++; }
		
	fclose(input_file);
}

//build minheap from table
//build huffman tree from minheap
//generate codes from huffman tree and insert into bit_string array
void Encoder::encode(string output_file_path)
{
	MinHeap * mh = new MinHeap();
	for (int i = 0; i < 256; i++)
	{
		if (frequency_table[i] != 0)
		{
			TreeNode * new_node = new TreeNode(i, frequency_table[i]);
			mh->insert(new_node);
		}
	}
	
	unique_chars = mh->getSize();

	HuffTree * huff = new HuffTree();
	huff->buildTree(mh);
	
	writeEncodedFile(output_file_path, huff);
}

void Encoder::writeEncodedFile(string output_file_path, HuffTree * huff)
{
	FILE * input_file = fopen((in_path).c_str(), "r");
	ofstream output_file((output_file_path).c_str(), ios::out | ios::binary);
	
	//Header here
	vector<char> characters;
	vector<int> frequencies;
	
	for (int i = 0; i < 256; i++)
		if (frequency_table[i] != 0)
		{
			characters.push_back(i);
			frequencies.push_back(frequency_table[i]);
		}	

	output_file.write(reinterpret_cast<char *>(&unique_chars), sizeof(unique_chars));
	
	for (int i = 0; i < characters.size(); i++)
	{
		output_file.write(reinterpret_cast<char *>(&characters[i]), sizeof(characters[i]));
		output_file.write(reinterpret_cast<char *>(&frequencies[i]), sizeof(frequencies[i]));
	}	
	
	//huff->printCodes();	
	//compressed text here
	//store converted data into one massive string to convert to byte chunks
	
	int c;
	string text_data;
	while ((c = fgetc(input_file)) != EOF)
	{ text_data.append(huff->getCharCode(c)); }
		
	if (text_data.length() % 8 != 0)
	{
		int pad_size = 8;
		while (pad_size < text_data.length())
			pad_size += 8;
		int pad_num = pad_size - text_data.length();
		for (int i = 0; i < pad_num; i++)
			text_data.append("0");
	}

	for (int i = 0; i < text_data.size(); i += 8)
	{ output_file.put(bitset<8>(text_data.substr(i, 8)).to_ulong()); }
}

Encoder::~Encoder()
{ delete this; }
