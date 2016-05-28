//https://en.wikipedia.org/wiki/Brainfuck
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>

//change to 1 for Windows, 0 for Unix
#define WINDOWS 1

#define initialSize 30000

typedef std::vector <char> MemoryBank;
char* highestMemoryCellUsed = nullptr;

void Run(std::string);

void Crash(std::string message)
{
	std::cout << message << std::endl;
	exit(1);
}

void HoldWindow()
{
	Run(",");
}

//dumps the contents of memory to stdout
void DumpMemory(MemoryBank &memory)
{
	int index = 0;
	while (index < (int)memory.size() && &memory[index] <= highestMemoryCellUsed)
	{
		printf("(%d:%c)\t", index, memory[index]);
		index++;
	}
}

//remove all non-brainfuck chars
std::string CleanCode(std::string code)
{
	std::string cleanCode = "";
	for (int i = 0; i < (int)code.size(); i++)
	{
		switch (code[i])
		{
		case '>':
		case '<':
		case '+':
		case '-':
		case '.':
		case ',':
		case ']':
		case '[':
		case '~':
			cleanCode += code[i];
		}
	}
	cleanCode.shrink_to_fit();
	return cleanCode;
}

void Run(std::string code)
{
	code = CleanCode(code);
	MemoryBank memory(initialSize, 0);
	int counter = 0;//program counter
	char* data = &memory[0];//pointer to where in memory we are
	highestMemoryCellUsed = data;
	while (counter <= (int)code.size())
	{
		char c = code[counter];
		switch (c)
		{
		case '>':
			data++;
			if (data == &memory[memory.size() - 1])
				memory.resize(memory.size() + initialSize, 0);
			if (data > highestMemoryCellUsed)
				highestMemoryCellUsed = data;
			break;
		case '<':
			if (data == &memory[0])
				Crash("Unable to move more left than cell 0");
			data--;
			break;
		case '+':
			++*data;
			break;
		case '-':
			--*data;
			break;
		case '.':
			putchar(*data);
			break;
		case ',':
			*data = getchar();
			break;
		case '[':
			if (*data == 0) //go forwards, start after matching ]
			{
				int bracketMatcher = 1;
				while (true)
				{
					counter++;
					if (counter == (int)memory.size())
						Crash("Mismatched [ and ] characters");
					if (code[counter] == '[')
					{
						bracketMatcher++;
					}
					if (code[counter] == ']')
					{
						bracketMatcher--;
						if (bracketMatcher == 0)
							break;
					}
				}
			}
			break;
		case ']':
			if (*data != 0) //go backwards, start after matching [
			{
				int bracketMatcher = 1;
				while (true)
				{
					counter--;
					if (counter < 0)
						Crash("Mismatched [ and ] characters");
					if (code[counter] == ']')
					{
						bracketMatcher++;
					}
					if (code[counter] == '[')
					{
						bracketMatcher--;
						if (bracketMatcher == 0)
							break;
					}
				}
			}
			break;
		case '~'://not in the brainfuck spec, but helpful for me
			DumpMemory(memory);
		default:
			//ignore all other characters.
			break;
		}
		counter++;
	}
}

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
		std::cin.ignore(0x7fffffff);
		Run(allCode);
	}
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
#if _DEBUG
	HoldWindow();
#endif
}