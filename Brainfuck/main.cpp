//https://en.wikipedia.org/wiki/Brainfuck
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "Brainfuck.h"
#include "MindBlown.h"

bool EndsWith(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) 
	{
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else 
	{
		return false;
	}
}

void PrintUsage()
{
	std::cout << "Usage: brainfuck[.exe] filename" << std::endl;
	std::cout << "If filename ends with .b or .bf, it will be run as brainfuck source." << std::endl;
	std::cout << "If filename ends with .mb, it will be compiled as mindblown source, then run." << std::endl;
	std::cout << "For documentation, please view readme at https://github.com/CodyIsAwesome/Brainfuck" << std::endl;
}

int main(int argc, char** argv)
{
	
	if (argc == 1) //no options, stdin
	{
		PrintUsage();
		exit(0);
	}
	else //parse args as filenames
	{
		std::string filename(argv[1]);
		std::ifstream file;
		file.open(filename, std::ifstream::in);
		if (file.is_open())
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::cout << "Output for " << filename << ":\n";
			if (EndsWith(filename, ".b") || EndsWith(filename, ".bf"))
			{
				Run(buffer.str());
			}
			else if (EndsWith(filename, ".mb"))
			{
				Run(Compile(buffer.str()));
			}
			else
			{

			}
		}
		else
		{
			std::cout << std::endl << "Error opening file " << filename << std::endl << std::endl;
		}
		file.close();
	}
}
