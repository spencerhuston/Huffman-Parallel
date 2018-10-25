#ifndef ENCODER_H
#define ENCODER_H
#include "HuffTree.h"
#include "MinHeap.h"
#include "TreeNode.h"
#include <string>
#include <atomic>

using namespace std;

class Encoder
{
	private:
		int NUM_THREADS;
		atomic<unsigned int> frequency_table[256];
		long lsize;
		string in_path;
		short unique_chars;

	public:
		//test_file_path is the input (decoded) file
		Encoder(string file_path, string thread_number); 
		
		//Fills up the frequency_table array where frequency_table[i] 
		//will contain the frequency of char with ASCII code i		
		void buildFrequencyTable(); 
		void outputTable();
		//Builds the min head and run the encoding algorithm
		void encode(string output_file_path);

		//Generates the encoded file and save it as output_file_path
		void writeEncodedFile(string output_file_path);
		void reverse(string & str);
		
		~Encoder();
};

#endif
