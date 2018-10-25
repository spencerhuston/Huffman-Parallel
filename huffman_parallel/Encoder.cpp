#include <thread>

#include <unistd.h>
#include <stdio.h>

#include <iostream>
#include <fstream>

#include <bitset>
#include <vector>
#include <chrono>

#include "Encoder.h"


// buffer : read-in buffer from input file, saved for use in writeEncodedFile()
// huff : global HuffTree object for encoding, used in encode() and build_text_data() 
unsigned char * buffer;
HuffTree * huff;

// sections : start points for each thread to operate on in data
// section_sizes : size of data for each thread to operate on
// appends : encoded data done by each thread
// text_data : encoded data from original file contents
vector<long> sections;
vector<long> section_sizes;
vector<string> appends;
string text_data;

// full_bitset : compressed binary form of text_data, done by each ith thread
vector<string> full_bitset;

/*
 * Encoder's only constructor, takes as input a string for the file path for the input file (args[3])
 * and a string for number of threads to run (args[2])
 */
Encoder::Encoder(string input_file_path, string thread_number)
{ 
	in_path = input_file_path; 
	NUM_THREADS = stoi(thread_number);
}

/*
 * Builds histogram of occurring byte frequencies
 * and saves global buffer 
 */
void Encoder::buildFrequencyTable()
{
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

/*
 * Constructs MinHeap data structure from histogram, followed by construction of HuffTree to build character codes
 * Takes file path for output file (args[4])
 */
void Encoder::encode(string output_file_path)
{
	MinHeap * mh = new MinHeap();
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

	writeEncodedFile(output_file_path);
}

/*
 * Called by threads in writeEncodedFile(), builds chuncks of encoded text_data 
 * Input is an integer specifying which thread is operating on it
 */
void build_text_data(int i)
{
	for (int j = sections[i]; j < sections[i] + section_sizes[i]; j++)
	{ appends[i] += huff->getCharCode(buffer[j]); }
}

/*
 * Called by threads in writeEncodedFile(), compresses text_data to binary form
 * Input is an integer specifying which thread is operating on it
 */
void build_bitset(int i)
{
	for (int j = sections[i]; j < sections[i] + section_sizes[i]; j += 8) 
		full_bitset[i] += (char)(bitset<8>(text_data.substr(j, 8)).to_ulong());
}

/*
 * The bulk of Encoder.cpp, takes as input the output file's path.
 * Builds encoded data in parallel by NUM_THREADS partitions,
 * then compresses that into a binary form by the same amount of partitions
 * then writes that out to the output file
 */
void Encoder::writeEncodedFile(string output_file_path)
{
	std::vector<std::thread> threads(NUM_THREADS);

	// partition data by NUM_THREADS
	int section_size = lsize/NUM_THREADS;
	int remainder = lsize - (section_size * NUM_THREADS);
		
	// assign starting points and data sizes for each thread
	for (int i = 0; i < NUM_THREADS; i++)
	{
		sections.push_back(section_size * i);
		if (remainder != 0 && i == NUM_THREADS - 1)
			section_sizes.push_back(section_size + remainder);
		else
			section_sizes.push_back(section_size);
	}	

	appends.resize(NUM_THREADS);

	// launch, run, and join threads, then append data to single text_data string
	for (int i = 0; i < NUM_THREADS; i++)
		threads[i] = thread(build_text_data, i);

	for (auto &t: threads)
		t.join();

	for (int i = 0; i < appends.size(); i++)	
		text_data += appends[i];

	// pad text_data with 0's to be modulo 8 for converesion to binary
	while (text_data.length() % 8 != 0)
	{ text_data.append("0"); }

	// partition text_data by NUM_THREADS
	remainder = text_data.size() % (NUM_THREADS * 8);
	section_size = (text_data.size() - remainder)/NUM_THREADS;
	
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

	// launch, run, and join threads building the bitsets
	for (int i = 0; i < threads.size(); i++)
		threads[i] = thread(build_bitset, i);

	for (auto &t: threads)
		t.join();

	ofstream output_file((output_file_path).c_str(), ios::out | ios::binary);

	// write header to output file for use in decoding
	output_file.write(reinterpret_cast<char *>(&unique_chars), sizeof(unique_chars));
	for (int i = 0; i < 256; i++)
		if (frequency_table[i] != 0)
		{
			char ic = (char)i;
			output_file.write(reinterpret_cast<char *>(&ic), 1);
			output_file.write(reinterpret_cast<char *>(&frequency_table[i]), 4);
		}	

	// write bitsets to output file
	for (int i = 0; i < full_bitset.size(); i++)
	{ output_file.write(full_bitset[i].c_str(), full_bitset[i].length()); }
	
	output_file.close();	

	std::cout << "Compression: " << 100 * ( 1 - ((( (double)text_data.length() ) / 8) / (double)lsize) ) << "%\n";
}

Encoder::~Encoder()
{ delete this; }
