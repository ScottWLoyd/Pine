#ifndef __PARSER_H
#define __PARSER_H

#include <vector>
#include "Token.h"

struct Parser
{
	Tokenizer tokenizer;
	std::vector<Token> tokens;
};

void Parse(Parser* parser);

#endif
