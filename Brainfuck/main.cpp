//https://en.wikipedia.org/wiki/Brainfuck
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "Brainfuck.h"
#include "MindBlown.h"

//change to 1 for Windows, 0 for Unix
#define WINDOWS 1

int main(int argc, char** argv)
{
	
	if (argc == 1) //no options, stdin
	{
		std::cout << "Enter your brainfuck code, and enter EOF ";
#if WINDOWS
		std::cout << "(Ctrl + Z)";
#else
		std::cout << "(Ctrl + D)";
#endif
		std::cout << ":\n";
		std::string line;
		std::string allCode;
		while (std::getline(std::cin, line))
		{
			allCode += line;
		}
		std::cin.ignore(0x7fffffff); //flush the EOF left in the stream from entry
		//Run(allCode);
		Run(Compile(allCode));
		int breakhere = 0;	
	}
	/*
	else //parse args as filenames
	{
		for (int i = 1; i < argc; i++)
		{
			std::string filename(argv[i]);
			std::ifstream file;
			file.open(filename, std::ifstream::in);
			if (file.is_open())
			{
				std::stringstream buffer;
				buffer << file.rdbuf();
				std::cout << "Output for " << filename << ":\n";
				Run(buffer.str());
			}
			else
			{
				std::cout << std::endl << "Error opening file " << filename << std::endl << std::endl;
			}
			file.close();
		}
	}
	*/
}
