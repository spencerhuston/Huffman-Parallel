#include <iostream>
#include <string>
#include <chrono>
#include "MinHeap.h"
#include "Encoder.h"
#include "Decoder.h"

using namespace std;

void printHelp(){
	cout << "Invalid arguments." << endl;
	cout << "Type \"huffman [mode] [thread number] [arguments]\" where mode and its arguments are one of the following:" << endl;
	cout << "-e [thread number] [src] [dest]: encode the source file and place the result into the specified destination" << endl;
	cout << "-d [thread number] [src] [dest]: decode the source file and place the result into the specified destination" << endl;
	exit(1);
}


int main (int argc, char** argv){
	if (argc != 5) 
		printHelp();
	else
	{
		if (string(argv[1]) == "-e")
		{
			auto start = chrono::high_resolution_clock::now();

			//ENCODE
			Encoder * compress = new Encoder(argv[3], argv[2]);
			compress->buildFrequencyTable();
			compress->encode(argv[4]);	

			auto finish = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed = finish - start;
			cout << "Total: " << elapsed.count() << endl;
		}
		else if (string(argv[1]) == "-d")
		{
			//DECODE
			Decoder * decompress = new Decoder(argv[3]);
			decompress->buildFrequencyTableFromFile();
			decompress->decode(argv[3]);		
			delete decompress;	
		}
		else
			printHelp();
	}

	return 0;
}
