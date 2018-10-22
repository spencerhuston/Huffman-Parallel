#ifndef ENCODER_H
#define ENCODER_H
#include "HuffTree.h"
#include "MinHeap.h"
#include "TreeNode.h"
#include <string>

using namespace std;

class Encoder
{
	private:
		unsigned frequency_table[256];
		string in_path;
		short unique_chars;
		// You need to add more class memeber and methods
	public:
		//test_file_path is the input (decoded) file
		Encoder(string file_path); 
		
		//Fills up the frequency_table array where frequency_table[i] 
		//will contain the frequency of char with ASCII code i		
		void buildFrequencyTable(); 
		void outputTable();
		//Builds the min head and run the encoding algorithm
		void encode(string output_file_path);

		//Generates the encoded file and save it as output_file_path
		void writeEncodedFile(string output_file_path, HuffTree * huff);
		void reverse(string & str);
		
		~Encoder();
};

#endif
