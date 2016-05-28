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

static const std::string MindBlowKeywords[] =
{
	"right",
	"left",
	"up",
	"down",
	"out",
	"in",
	"loop",
	"end",
	"define",
	"memdump",
};

typedef enum TokenType
{
	Unknown,
	Keyword,
	RawBrainfuck,
	Formatting,
	Constant,
	Definition
};

struct Token
{
	std::string value;
	TokenType type;

	Token(std::string value);
	Token(std::string value, TokenType type);
	Token(char value);
	Token(char value, TokenType type);
};

Token::Token(std::string value)
{
	this->value = value;
	type = TokenType::Unknown;
}

Token::Token(std::string value, TokenType type)
{
	this->value = value;
	this->type = type;
}

Token::Token(char value)
{
	this->value = "";
	this->value += value;
	this->type = TokenType::Unknown;
}

Token::Token(char value, TokenType type)
{
	this->value = "";
	this->value += value;
	this->type = type;
}

void Crash(std::string message)
{
	std::cout << message << std::endl;
	exit(1);
}

bool IsInt(std::string val)
{
	for (char c : val)
	{
		if (c >= '0' && c <= '9')
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool IsDefinition(std::vector<Token> &tokens, unsigned int checkThisIndex)
{
	return false; //not implemented yet
}

bool IsKeyword(std::string val)
{
	for (std::string keyword : MindBlowKeywords)
	{
		if (val.compare(keyword) == 0)
			return true;
	}
	return false;
}

std::vector<Token> Tokenize(std::string code)
{
	std::vector<Token> tokens;
	std::string longerToken = "";
	for (char c : code)
	{
		switch (c)
		{
		//throw away whitespace
		case ' ':
		case '\t':
		case '\n':
		case '\r\n':
			if (longerToken.compare("") != 0)
			{
				tokens.push_back(Token(longerToken));
				longerToken = "";
			}
			break;
		//brainfuck
		case '<':
		case '>':
		case '+':
		case '-':
		case '.':
		case ',':
		case '[':
		case ']':
		case '~':
			if (longerToken.compare("") != 0)
			{
				tokens.push_back(Token(longerToken));
				longerToken = "";
			}
			tokens.push_back(Token(c, TokenType::RawBrainfuck));
			break;
		//formatting
		case '(':
		case ')':
		case '{':
		case '}':
		case ';':
			if (longerToken.compare("") != 0)
			{
				tokens.push_back(Token(longerToken));
				longerToken = "";
			}
			tokens.push_back(Token(c, TokenType::Formatting));
			break;
		//all other chars
		default:
			longerToken += c;
			break;
		}
	}
	return tokens;
}

//Takes in unidentified tokens, moves iterator forward if it needs to pull several tokens.
//it handles appending whatever tokens it takes
//returns new iterator position that it finished at, caller should continue at return val + 1
unsigned int Identify(std::vector<Token> &read, std::vector<Token> &write, unsigned int checkThisIndex)
{
	for (unsigned int i = checkThisIndex; i < read.size(); /*no iterator: manually do this in loop*/)
	{
		std::string thisTokenValue = read[i].value;
		if (IsKeyword(thisTokenValue))
		{
			write.push_back(Token(thisTokenValue, TokenType::Keyword));
			return i;
		}
		else if (IsInt(thisTokenValue))
		{
			write.push_back(Token(thisTokenValue, TokenType::Constant));
			return i;
		}
		else if (IsDefinition(read, i))
		{
			//add to symbol table
			//TODO: implement this
			return i;
		}
		else
		{
			//might be an undefined macro definition
			//TODO detect and handle this

			//it is bad, kill process
			Crash("Unknown element: " + thisTokenValue);
			return i;//never executed
		}
	}
}

//this function classifies valid code, and misclassifies invalid code
//
std::vector<Token> Classify(std::vector<Token> &tokens)
{
	std::vector<Token> classifiedTokens;
	bool inComment = false;
	for (unsigned int i = 0; i < tokens.size(); i++)
	{
		Token thisToken = tokens[i];
		switch (thisToken.type)
		{
			//unknown string, figure it out bro!
			case TokenType::Unknown:
				if (inComment) break;
				i = Identify(tokens, classifiedTokens, i);
				break;
			//we already know what it is, just add it
			case TokenType::Keyword:
			case TokenType::RawBrainfuck:
			case TokenType::Constant:
				if (inComment) break;
				classifiedTokens.push_back(thisToken);
				break;
			//use this to handle comments, building symbol table for defines
			case TokenType::Formatting:
				if (thisToken.value.compare("(") == 0) //start comment
				{
					if (inComment)
						Crash("Cannot have a '(' inside a comment block");
					inComment = true;
					break;
				}
				else if (thisToken.value.compare(")") == 0) //end comment
				{
					if (!inComment)
						Crash("Cannot have a ')' inside a comment block");
					inComment = false;
					break;
				}
				classifiedTokens.push_back(thisToken);
				break;
		}
	}
	return classifiedTokens;
}

std::string Translate(std::vector<Token> tokens)
{
	std::string translatedCode = "";
	for (unsigned int i = 0; i < tokens.size(); i++)
	{
		Token t = tokens[i];
		switch (t.type)
		{
		case TokenType::Keyword:
			if (t.value.compare("right") == 0)
			{
				translatedCode += ">";
			}
			else if (t.value.compare("left") == 0)
			{
				translatedCode += "<";
			}
			else if (t.value.compare("up") == 0)
			{
				translatedCode += "+";
			}
			else if (t.value.compare("down") == 0)
			{
				translatedCode += "-";
			}
			else if (t.value.compare("in") == 0)
			{
				translatedCode += ",";
			}
			else if (t.value.compare("out") == 0)
			{
				translatedCode += ".";
			}
			else if (t.value.compare("loop") == 0)
			{
				translatedCode += "[";
			}
			else if (t.value.compare("end") == 0)
			{
				translatedCode += "]";
			}
			else if (t.value.compare("define") == 0)
			{
				//TODO implement this
			}
			else if (t.value.compare("memdump") == 0)
			{
				translatedCode += "~";
			}
			break;
		case TokenType::RawBrainfuck:
			translatedCode += t.value;
			break;
		case TokenType::Formatting:
			//just throw this away
			break;
		case TokenType::Constant:
			Crash("Error: unexpected token: " + t.value);
			break;
		case TokenType::Definition:
			//TODO implement this
			break;
		case TokenType::Unknown:
		default:
			Crash("Unknown token entered the translation step: " + t.value);
			break;
		}
	}
	return translatedCode;
}

std::string Compile(std::string mindblowCode)
{
	std::vector<Token> tokens = Tokenize(mindblowCode);
	std::vector<Token> organizedTokens = Classify(tokens);
	std::string brainfuckCode = Translate(organizedTokens);
	return brainfuckCode;
}

void HoldWindow()
{
	Run(",");
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
	while (index < (int)memory.size() && &memory[index] <= highestMemoryCellUsed)
	{
		if (IsPrintable(memory[index]))
			printf("(%d:%c)\t", index, memory[index]);
		else
			printf("(%d:%d)\t", index, memory[index]);
		numPrintedThisLine++;
		if (numPrintedThisLine == 10)
		{
			numPrintedThisLine = 0;
			printf("\n");
		}
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
		std::cin.ignore(0x7fffffff); //flush the EOF left in the stream from entry
		//Run(allCode);
		Run(Compile(allCode));
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
#if _DEBUG
	HoldWindow();
#endif
}
