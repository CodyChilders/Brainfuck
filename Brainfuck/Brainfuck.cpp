#include <string>
#include <vector>
#include <iostream>
#include "Brainfuck.h"

#define initialSize 30000

typedef std::vector<char> MemoryBank;
char* highestMemoryCellUsed = nullptr;

void Crash(std::string message)
{
	std::cout << "Error: " << message << std::endl;
	exit(1);
}


bool IsPrintable(char c)
{
	if (c >= 32 && c <= 126)
		return true;
	else
		return false;
}

//dumps the contents of memory to stdout
void DumpMemory(MemoryBank &memory)
{
	int index = 0;
	int numPrintedThisLine = 0;
	printf("\n");
	while (index < (int)memory.size() && &memory[index] <= highestMemoryCellUsed)
	{
		if (IsPrintable(memory[index]))
			printf("(%3d:%3c) ", index, memory[index]);
		else
			printf("(%3d:%3d) ", index, ((unsigned char)(memory[index])));
		numPrintedThisLine++;
		if (numPrintedThisLine == 10)
		{
			numPrintedThisLine = 0;
			printf("\n");
		}
		index++;
	}
	printf("\n");
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

//This function runs the brainfuck code as is, stripping all non-brainfuck chars from input.
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
						Crash("Mismatched [ and ] characters.  Too many [ without enough ] to match.");
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
						Crash("Mismatched [ and ] characters.  Too many ] without enough [ to match.");
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
			break;
		default:
			//ignore all other characters.
			break;
		}
		counter++;
	}
}
