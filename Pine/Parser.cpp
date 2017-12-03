#include <vector>

#include "sl.h"
#include "Token.h"

#include "Parser.h"

void
Parse(Parser* parser)
{
	int parsing = true;

	while (parsing)
	{
		Token token = get_token(&parser->tokenizer);

		if (token.type == TOKEN_END_OF_STREAM)
		{
			parsing = false;
			break;
		}

		parser->tokens.push_back(token);

	}

	// DEBUG!!!
	for (u32 i = 0; i<parser->tokens.size(); i++)
	{
		Token token = parser->tokens[i];
		char* buf = (char*)malloc(token.len + 1);
		strncpy(buf, token.text, token.len);
		buf[token.len] = 0;
		printf("%d,%d: %s - %s\n", token.line, token.column, TokenType_Strings[token.type], buf);
		free(buf);
	}
}