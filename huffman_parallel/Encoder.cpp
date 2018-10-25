#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>
#include "Encoder.h"
#include <chrono>

unsigned char * buffer;
HuffTree * huff;

vector<long> sections;
vector<long> section_sizes;
vector<string> appends;
string text_data;

vector<string> full_bitset;

Encoder::Encoder(string input_file_path, string thread_number)
{ 
	in_path = input_file_path; 
	NUM_THREADS = stoi(thread_number);
}

//read from original file, store into buffer and construct frequency table
void Encoder::buildFrequencyTable()
{
	auto start = std::chrono::high_resolution_clock::now();
	FILE * input_file = fopen((in_path).c_str(), "r");
	
	fseek(input_file, 0, SEEK_END);
	lsize = ftell(input_file);
	rewind(input_file);	

	buffer = (unsigned char *) malloc (sizeof(char) * lsize);
	size_t result = fread(buffer, 1, lsize, input_file);
	fclose(input_file);

	for (int i = 0; i < result; i++)
		frequency_table[buffer[i]]++;
}

//build minheap from table
//build huffman tree from minheap
//generate codes from huffman tree and insert into bit_string array
void Encoder::encode(string output_file_path)
{
	MinHeap * mh = new MinHeap();
	//probably not the best to try to parallelize
	//means I have to make a thread-safe Heap == not fun
	for (int i = 0; i < 256; i++)
	{
		if (frequency_table[i])
		{
			TreeNode * new_node = new TreeNode(i, frequency_table[i]);
			mh->insert(new_node);
		}
	}

	unique_chars = mh->getSize();

	huff = new HuffTree();
	huff->buildTree(mh);

	writeEncodedFile(output_file_path, huff);
}

void build_text_data(int i)
{
	for (int j = sections[i]; j < sections[i] + section_sizes[i]; j++)
	{ appends[i] += huff->getCharCode(buffer[j]); }
}

void build_bitset(int i)
{
	for (int j = sections[i]; j < sections[i] + section_sizes[i]; j += 8) 
		full_bitset[i] += (char)(bitset<8>(text_data.substr(j, 8)).to_ulong());
}

//parallel version of original function
void Encoder::writeEncodedFile(string output_file_path, HuffTree * huff)
{
	std::vector<std::thread> threads(NUM_THREADS);

	//partition data from file per number of threads
	int section_size = lsize/NUM_THREADS;
	int remainder = lsize - (section_size * NUM_THREADS);
		
	for (int i = 0; i < NUM_THREADS; i++)
	{
		sections.push_back(section_size * i);
		if (remainder != 0 && i == NUM_THREADS - 1)
			section_sizes.push_back(section_size + remainder);
		else
			section_sizes.push_back(section_size);
	}	

	appends.resize(NUM_THREADS);

	//Create string of huffman codes
	for (int i = 0; i < NUM_THREADS; i++)
		threads[i] = thread(build_text_data, i);

	for (auto &t: threads)
		t.join();

	for (int i = 0; i < appends.size(); i++)	
		text_data += appends[i];

	//pad data to fit % 8 for bytes
	while (text_data.length() % 8 != 0)
	{ text_data.append("0"); }

	//partition huffman code string
	remainder = text_data.size() % (NUM_THREADS * 8);
	section_size = (text_data.size() - remainder)/NUM_THREADS;
	
	//extra thread that takes care of leftover bits
	int thread_size = (remainder != 0) ? NUM_THREADS + 1 : NUM_THREADS;

	full_bitset.resize(thread_size);
	
	sections.clear();
	section_sizes.clear();

	threads.empty();
	threads.resize(thread_size);

	for (int i = 0; i < full_bitset.size(); i++)
	{
		sections.push_back(section_size * i);
		if (remainder != 0 && i == full_bitset.size() - 1)
			 section_sizes.push_back(remainder);
		else
			 section_sizes.push_back(section_size);
	}

	//create chunks of binary data from huffman codes
	for (int i = 0; i < threads.size(); i++)
		threads[i] = thread(build_bitset, i);

	for (auto &t: threads)
		t.join();

	ofstream output_file((output_file_path).c_str(), ios::out | ios::binary);

	//write header data to file
	output_file.write(reinterpret_cast<char *>(&unique_chars), sizeof(unique_chars));
	for (int i = 0; i < 256; i++)
		if (frequency_table[i] != 0)
		{
			char ic = (char)i;
			output_file.write(reinterpret_cast<char *>(&ic), 1);
			output_file.write(reinterpret_cast<char *>(&frequency_table[i]), 4);
		}	

	//write chunks of huffman codes in binary to file
	//improvement over the original due to writing in large chunks rather than character by character
	for (int i = 0; i < full_bitset.size(); i++)
	{ output_file.write(full_bitset[i].c_str(), full_bitset[i].length()); }
	
	output_file.close();	

	std::cout << "Compression: " << 100 * ( 1 - ((( (double)text_data.length() ) / 8) / (double)lsize) ) << "%\n";
}

Encoder::~Encoder()
{ delete this; }
