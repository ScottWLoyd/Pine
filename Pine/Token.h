#ifndef __TOKEN_H
#define __TOKEN_H

#include "sl.h"

typedef struct Tokenizer
{
	char *start, *curr, *end;
	char *line_start;
	size_t line;
} Tokenizer;

internal Tokenizer
make_tokenizer(read_file_result FileContents)
{
	Tokenizer t = { 0 };
	t.start = (char*)FileContents.contents;
	t.curr = t.start;
	t.end = t.start + FileContents.size;
	t.line_start = t.start;
	t.line = 1;

	return t;
}

typedef enum TokenType {
	TOKEN_UNKNOWN,

	TOKEN_COMMENT,
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_INTEGER,
	TOKEN_FLOAT,

	TOKEN_OPEN_PAREN,       // (
	TOKEN_CLOSE_PAREN,      // )
	TOKEN_OPEN_BRACKET,     // [
	TOKEN_CLOSE_BRACKET,    // ]
	TOKEN_OPEN_BRACE,       // {
	TOKEN_CLOSE_BRACE,      // }
	TOKEN_COMMA,            // ,
	TOKEN_SEMICOLON,        // ;
	TOKEN_HASH,             // #
	TOKEN_BACKSLASH,        // \ `
	TOKEN_AT,               // @
	TOKEN_QUESTION,         // ?

	TOKEN_COLON,            // :
	TOKEN_COLON_COLON,      // ::
	TOKEN_EQUALS,           // =
	TOKEN_EQUALS_EQUALS,    // ==
	TOKEN_DOT,              // .
	TOKEN_DOT_DOT,          // ..

	TOKEN_PLUS,             // +
	TOKEN_PLUS_PLUS,        // ++
	TOKEN_PLUS_EQUALS,      // +=

	TOKEN_MINUS,            // -
	TOKEN_MINUS_MINUS,      // --
	TOKEN_MINUS_EQUALS,     // -=
	TOKEN_MINUS_GT,         // ->

	TOKEN_AND,              // &
	TOKEN_AND_AND,          // &&
	TOKEN_AND_EQUALS,       // &=
	TOKEN_PIPE,             // |
	TOKEN_PIPE_PIPE,        // ||
	TOKEN_PIPE_EQUALS,      // |=   
	TOKEN_LT,               // <
	TOKEN_LT_LT,            // <<
	TOKEN_LE,               // <=
	TOKEN_GT,               // >
	TOKEN_GT_GT,            // >>
	TOKEN_GE,               // >=

	TOKEN_PERCENT,          // %
	TOKEN_PERCENT_EQUALS,   // %=
	TOKEN_ASTERISK,         // *
	TOKEN_ASTERISK_EQUALS,  // *=   
	TOKEN_CARAT,            // ^
	TOKEN_CARAT_EQUALS,     // ^=
	TOKEN_BANG,             // !
	TOKEN_BANG_EQUALS,      // != 
	TOKEN_TILDE,            // ~
	TOKEN_TILDE_EQUALS,     // ~=

							// Has to be handled separately because of comment syntax
	TOKEN_SLASH,            // /
	TOKEN_SLASH_EQUALS,     // /=

	TOKEN_END_OF_STREAM,

	TOKEN_COUNT,
} TokenType;

extern char* TokenType_Strings[TOKEN_COUNT];

typedef struct Token
{
	TokenType type;
	char* text;
	size_t len;
	size_t line;
	size_t column;
} Token;


Token get_token(Tokenizer* t);

#endif