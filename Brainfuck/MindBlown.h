#pragma once
#include <string>

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

std::string Compile(std::string mindblowCode);
