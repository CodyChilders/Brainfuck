#include <string>
#include <vector>
#include "Brainfuck.h"
#include "MindBlown.h"

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
				int numToAdd = 1;
				if (tokens[i + 1].type == TokenType::Constant)
				{
					numToAdd = atoi(tokens[++i].value.c_str());
				}
				else if (tokens[i + 1].value.compare(";") == 0)
				{
					//this is valid, but nothing needs to be done.  pull it off the queue and keep going
				}
				else
				{
					//no other characters are expected here
					Crash("Unexpected token: (" + tokens[i + 1].value + ") after 'right'.");
				}
				for (int i = 0; i < numToAdd; i++)
					translatedCode += ">";
			}
			else if (t.value.compare("left") == 0)
			{
				int numToAdd = 1;
				if (tokens[i + 1].type == TokenType::Constant)
				{
					numToAdd = atoi(tokens[++i].value.c_str());
				}
				else if (tokens[i + 1].value.compare(";") == 0)
				{
					//this is valid, but nothing needs to be done.  pull it off the queue and keep going
				}
				else
				{
					//no other characters are expected here
					Crash("Unexpected token: (" + tokens[i + 1].value + ") after 'left'.");
				}
				for (int i = 0; i < numToAdd; i++)
					translatedCode += "<";
			}
			else if (t.value.compare("up") == 0)
			{
				int numToAdd = 1;
				if (tokens[i + 1].type == TokenType::Constant)
				{
					numToAdd = atoi(tokens[++i].value.c_str());
				}
				else if (tokens[i + 1].value.compare(";") == 0)
				{
					//this is valid, but nothing needs to be done.  pull it off the queue and keep going
				}
				else
				{
					//no other characters are expected here
					Crash("Unexpected token: (" + tokens[i + 1].value + ") after 'up'.");
				}
				for (int i = 0; i < numToAdd; i++)
					translatedCode += "+";
			}
			else if (t.value.compare("down") == 0)
			{
				int numToAdd = 1;
				if (tokens[i + 1].type == TokenType::Constant)
				{
					numToAdd = atoi(tokens[++i].value.c_str());
				}
				else if (tokens[i + 1].value.compare(";") == 0)
				{
					//this is valid, but nothing needs to be done.  pull it off the queue and keep going
				}
				else
				{
					//no other characters are expected here
					Crash("Unexpected token: (" + tokens[i + 1].value + ") after 'down'.");
				}
				for (int i = 0; i < numToAdd; i++)
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
			//constants aren't inherently an error, but only follow a keyword, so it is the keyword's responsibility to process it
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
