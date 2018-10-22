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

	buffer = (unsigned char *) malloc (sizeof(char) *lsize);
	size_t result = fread(buffer, 1, lsize, input_file);
	fclose(input_file);

	for (int i = 0; i < result; i++)
		frequency_table[buffer[i]]++;

	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Build Frequency Table: " << elapsed.count() << '\n';
}

//build minheap from table
//build huffman tree from minheap
//generate codes from huffman tree and insert into bit_string array
void Encoder::encode(string output_file_path)
{
	auto start = std::chrono::high_resolution_clock::now();
	MinHeap * mh = new MinHeap();
	//probably not the best to try to parallelize
	//means I have to make a thread-safe Heap == not fun
	for (int i = 0; i < 256; i++)
	{
		if (frequency_table[i] != 0)
		{
			TreeNode * new_node = new TreeNode(i, frequency_table[i]);
			mh->insert(new_node);
		}
	}

	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Build MinHeap: " << elapsed.count() << '\n';
	
	unique_chars = mh->getSize();

	start = std::chrono::high_resolution_clock::now();	
	huff = new HuffTree();
	huff->buildTree(mh);

	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - start;
        std::cout << "Build Huffman Tree: " << elapsed.count() << '\n';

	start = std::chrono::high_resolution_clock::now();
	writeEncodedFile(output_file_path, huff);
	
	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - start;
        std::cout << "All of writeEncodedFile: " << elapsed.count() << '\n';
}

vector<long> lsections;
vector<long> lsection_sizes;
vector<string> appends;
HuffTree * build_huff = huff;
string text_data;

void build_text_data(int i)
{
	string local_text_data;
	for (int j = lsections[i]; j < lsections[i] + lsection_sizes[i]; j++)
	{ local_text_data.append(huff->getCharCode(buffer[j])); }
	appends[i] = local_text_data;
}

vector<long> sections;
vector<long> section_sizes;
vector<string> full_bitset;

void build_bitset(int i)
{
	for (int j = sections[i]; j < sections[i] + section_sizes[i]; j += 8) 
		full_bitset[i] += (char)(bitset<8>(text_data.substr(j, 8)).to_ulong());
}

//parallel version of original function
void Encoder::writeEncodedFile(string output_file_path, HuffTree * huff)
{
	std::vector<std::thread> threads(NUM_THREADS);

	auto start = std::chrono::high_resolution_clock::now();
	//partition data from file per number of threads
	bool lremainder_exists = (lsize % NUM_THREADS != 0) ? true : false;
	int lsection_size = lsize/NUM_THREADS;
	int lremainder = (lremainder_exists) ? lsize - (lsection_size * NUM_THREADS) : 0;
		
	for (int i = 0; i < NUM_THREADS; i++)
	{
		lsections.push_back(lsection_size * i);
		if (lremainder_exists && i == NUM_THREADS - 1)
			lsection_sizes.push_back(lsection_size + lremainder);
		else
			lsection_sizes.push_back(lsection_size);
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
	if (text_data.length() % 8 != 0)
	{
		int pad_size = 0;
		for (pad_size = 8; pad_size < text_data.length(); pad_size += 8) {}
		int pad_num = pad_size - text_data.length();
		for (int i = 0; i < pad_num; i++)
			text_data.append("0");
	}

	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
        std::cout << "\tBuild huffman code text: " << elapsed.count() << '\n';

	start = std::chrono::high_resolution_clock::now();
	//partition huffman code string
	int remainder = text_data.size() % (NUM_THREADS * 8);
	long section_size = (text_data.size() - remainder)/NUM_THREADS;
	
	//extra thread that takes care of leftover bits
	int thread_size = (remainder != 0) ? NUM_THREADS + 1 : NUM_THREADS;

	full_bitset.resize(thread_size);
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

	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - start;
        std::cout << "\tConvert code to binary: " << elapsed.count() << '\n';

	ofstream output_file((output_file_path).c_str(), ios::out | ios::binary);

	start = std::chrono::high_resolution_clock::now();
	//write header data to file
	output_file.write(reinterpret_cast<char *>(&unique_chars), sizeof(unique_chars));
	for (int i = 0; i < 256; i++)
		if (frequency_table[i] != 0)
		{
			char ic = (char)i;
			output_file.write(reinterpret_cast<char *>(&ic), 1);
			output_file.write(reinterpret_cast<char *>(&frequency_table[i]), 4);
		}	

	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - start;
        std::cout << "\tWrite header: " << elapsed.count() << '\n';

	//write chunks of huffman codes in binary to file
	//improvement over the original due to writing in large chunks rather than character by character
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < full_bitset.size(); i++)
	{ output_file.write(full_bitset[i].c_str(), full_bitset[i].length()); }
	
	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - start;
        std::cout << "\tWrite to file: " << elapsed.count() << '\n';

	output_file.close();	
}

Encoder::~Encoder()
{ delete this; }
