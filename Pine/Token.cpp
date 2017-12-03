
#include "Token.h"

char* TokenType_Strings[TOKEN_COUNT] =
{
	"TOKEN_UNKNOWN",
	"TOKEN_COMMENT",
	"TOKEN_IDENTIFIER",
	"TOKEN_STRING",
	"TOKEN_INTEGER",
	"TOKEN_FLOAT",
	"TOKEN_OPEN_PAREN",
	"TOKEN_CLOSE_PAREN",
	"TOKEN_OPEN_BRACKET",
	"TOKEN_CLOSE_BRACKET",
	"TOKEN_OPEN_BRACE",
	"TOKEN_CLOSE_BRACE" ,
	"TOKEN_COMMA",
	"TOKEN_SEMICOLON",
	"TOKEN_HASH",
	"TOKEN_BACKSLASH" ,
	"TOKEN_AT",
	"TOKEN_QUESTION",
	"TOKEN_COLON",
	"TOKEN_COLON_COLON",
	"TOKEN_EQUALS",
	"TOKEN_EQUALS_EQUALS",
	"TOKEN_DOT",
	"TOKEN_DOT_DOT",
	"TOKEN_PLUS",
	"TOKEN_PLUS_PLUS",
	"TOKEN_PLUS_EQUALS",
	"TOKEN_MINUS",
	"TOKEN_MINUS_MINUS",
	"TOKEN_MINUS_EQUALS",
	"TOKEN_MINUS_GT",
	"TOKEN_AND",
	"TOKEN_AND_AND",
	"TOKEN_AND_EQUALS",
	"TOKEN_PIPE",
	"TOKEN_PIPE_PIPE",
	"TOKEN_PIPE_EQUALS",
	"TOKEN_LT",
	"TOKEN_LT_LT",
	"TOKEN_LE",
	"TOKEN_GT",
	"TOKEN_GT_GT",
	"TOKEN_GE",
	"TOKEN_PERCENT",
	"TOKEN_PERCENT_EQUALS",
	"TOKEN_ASTERISK",
	"TOKEN_ASTERISK_EQUALS",
	"TOKEN_CARAT",
	"TOKEN_CARAT_EQUALS",
	"TOKEN_BANG",
	"TOKEN_BANG_EQUALS",
	"TOKEN_TILDE",
	"TOKEN_TILDE_EQUALS",
	"TOKEN_SLASH",
	"TOKEN_SLASH_EQUALS",
	"TOKEN_END_OF_STREAM"
};

inline internal bool32
try_increment_line(Tokenizer* t)
{
	if (t->curr[0] == '\n')
	{
		t->line++;
		t->line_start = ++t->curr;
		return true;
	}
	return false;
}

internal bool32
skip_space(Tokenizer* t)
{
	for (;;)
	{
		if (is_space(t->curr[0]) && t->curr[0] != '\n')
		{
			t->curr++;
		}
		else if (t->curr[0] == '\n')
		{
			try_increment_line(t);
		}
		else if (t->curr[0] == '\0' || t->curr >= t->end)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

inline internal void
trim_space(Token* token)
{
	char* end = token->text + token->len - 1;
	while (is_space(*end))
	{
		end--;
	}
	token->len = end - token->text + 1;
}

Token
get_token(Tokenizer* t)
{
	Token token = { TOKEN_UNKNOWN };
	char c;

	if (!skip_space(t))
	{
		token.type = TOKEN_END_OF_STREAM;
		return token;
	}

	token.text = t->curr;
	token.len = 1;
	token.line = t->line;
	token.column = token.text - t->line_start + 1;
	c = *t->curr++;

	switch (c)
	{
		// TODO: finish implementing all of these cases
		// TODO: _TEST_ all of these cases!
	case '\0':
		token.type = TOKEN_END_OF_STREAM;
		t->curr--;
		break;

	case '(': token.type = TOKEN_OPEN_PAREN; break;
	case ')': token.type = TOKEN_CLOSE_PAREN; break;
	case '[': token.type = TOKEN_OPEN_BRACKET; break;
	case ']': token.type = TOKEN_CLOSE_BRACKET; break;
	case '{': token.type = TOKEN_OPEN_BRACE; break;
	case '}': token.type = TOKEN_CLOSE_BRACE; break;
	case ',': token.type = TOKEN_COMMA; break;
	case ';': token.type = TOKEN_SEMICOLON; break;
	case '#': token.type = TOKEN_HASH; break;
	case '\\': token.type = TOKEN_BACKSLASH; break;
	case '@': token.type = TOKEN_AT; break;
	case '?': token.type = TOKEN_QUESTION; break;


#define DOUBLE_TOKEN(c, single_token, double_token) \
      case c:  \
      { \
         if (t->curr[0] == c) \
         { \
            token.type = double_token; \
            token.len = 2; \
            t->curr += 2; \
         } \
         else \
         { \
            token.type = single_token;  \
         } \
      } break;

		DOUBLE_TOKEN(':', TOKEN_COLON, TOKEN_COLON_COLON);
		DOUBLE_TOKEN('=', TOKEN_EQUALS, TOKEN_EQUALS_EQUALS);
		DOUBLE_TOKEN('.', TOKEN_DOT, TOKEN_DOT_DOT);
#undef DOUBLE_TOKEN

#define DOUBLE_ASSIGN_TOKEN(c, a, single_token, double_token, assign_token) \
      case c:  \
      { \
         if (t->curr[0] == c) \
         { \
            token.type = double_token; \
            token.len = 2; \
            t->curr += 2; \
         } \
         else if (t->curr[0] == a) \
         { \
            token.type = assign_token; \
            token.len = 2; \
            t->curr += 2; \
         } \
         else \
         { \
            token.type = single_token;  \
         } \
      } break; 

		DOUBLE_ASSIGN_TOKEN('+', '=', TOKEN_PLUS, TOKEN_PLUS_PLUS, TOKEN_PLUS_EQUALS);
		DOUBLE_ASSIGN_TOKEN('&', '=', TOKEN_AND, TOKEN_AND_AND, TOKEN_AND_EQUALS);
		DOUBLE_ASSIGN_TOKEN('|', '=', TOKEN_PIPE, TOKEN_PIPE_PIPE, TOKEN_PIPE_EQUALS);
		DOUBLE_ASSIGN_TOKEN('<', '=', TOKEN_LT, TOKEN_LT_LT, TOKEN_LE);
		DOUBLE_ASSIGN_TOKEN('>', '=', TOKEN_GT, TOKEN_GT_GT, TOKEN_GE);
#undef DOUBLE_ASSIGN_TOKEN

	case '-':
	{
		if (t->curr[0] == '-')
		{
			token.type = TOKEN_MINUS_MINUS;
			token.len = 2;
			t->curr += 2;
		}
		else if (t->curr[0] == '=')
		{
			token.type = TOKEN_MINUS_EQUALS;
			token.len = 2;
			t->curr += 2;
		}
		else if (t->curr[0] == '>')
		{
			token.type = TOKEN_MINUS_GT;
			token.len = 2;
			t->curr += 2;
		}
		else
		{
			token.type = TOKEN_MINUS;
		}
	} break;

#define SINGLE_ASSIGN_TOKEN(c, a, single_token, assign_token) \
      case c:  \
      { \
         if (t->curr[0] == a) \
         { \
            token.type = assign_token; \
            token.len = 2; \
            t->curr += 2; \
         } \
         else \
         { \
            token.type = single_token;  \
         } \
      } break;    

	SINGLE_ASSIGN_TOKEN('%', '=', TOKEN_PERCENT, TOKEN_PERCENT_EQUALS);
	SINGLE_ASSIGN_TOKEN('*', '=', TOKEN_ASTERISK, TOKEN_ASTERISK_EQUALS);
	SINGLE_ASSIGN_TOKEN('^', '=', TOKEN_CARAT, TOKEN_CARAT_EQUALS);
	SINGLE_ASSIGN_TOKEN('!', '=', TOKEN_BANG, TOKEN_BANG_EQUALS);
	SINGLE_ASSIGN_TOKEN('~', '=', TOKEN_TILDE, TOKEN_TILDE_EQUALS);
#undef SINGLE_ASSIGN_TOKEN      


	case '"': {
		// String literal
		token.type = TOKEN_STRING;
		token.text = t->curr;

		while (t->curr[0] && t->curr[0] != '"')
		{
			if (t->curr[0] == '\\' && t->curr[1])
				t->curr++;
			t->curr++;
		}

		token.len = t->curr - token.text;
		if (t->curr[0] == '"')
			t->curr++;
	} break;

	case '/':
	{
		// Single line comment
		if (t->curr[0] == '/')
		{
			t->curr++;
			skip_space(t);

			token.text = t->curr;
			token.type = TOKEN_COMMENT;
			while (t->curr[0] && t->curr[0] != '\n' && t->curr < t->end)
			{
				t->curr++;
			}

			token.len = t->curr - token.text;
			trim_space(&token);
		}
		else if (t->curr[0] == '*')
		{
			// Multi-line comment
			size_t comment_scope = 1;
			t->curr++;
			skip_space(t);

			token.text = t->curr;
			token.type = TOKEN_COMMENT;
			while (t->curr[0] &&
				comment_scope > 0 &&
				(t->curr < t->end))
			{
				if (t->curr[0] == '/' && t->curr[1] == '*')
					comment_scope++;
				if (t->curr[0] == '*' && t->curr[1] == '/')
					comment_scope--;

				if (!try_increment_line(t))
					t->curr++;
			}

			token.len = t->curr - 1 - token.text;
			trim_space(&token);

			if (t->curr[-1] == '*' && t->curr[0] == '/')
			{
				t->curr += 2;
			}
		}
		else if (t->curr[0] == '=')
		{
			// Divide-Equals
			token.type = TOKEN_SLASH_EQUALS;
			token.len = 2;
		}
		else
		{
			// Divide
			token.type = TOKEN_SLASH;
			token.len = 2;
		}

	} break;

	default:
	{
		if (is_alpha(c) || c == '_')
		{
			token.type = TOKEN_IDENTIFIER;

			while (is_alnum(t->curr[0]) || t->curr[0] == '_')
			{
				t->curr++;
			}

			token.len = t->curr - token.text;
		}
		else if (is_num(c))
		{
			token.type = TOKEN_INTEGER;

			bool32 IsHex = false;
			bool32 IsBinary = false;
			bool32 IsFloat = false;
			if (c == '0' && t->curr[0] == 'x')
				IsHex = true;
			if (c == '0' && t->curr[0] == 'b')
				IsBinary = true;
			if (c == '0' && t->curr[0] == '.')
				IsFloat = true;

			int base = 10;
			if (IsHex)
			{
				base = 16;
				t->curr++;
			}
			else if (IsBinary)
			{
				base = 2;
				t->curr++;
			}
			else if (IsFloat)
			{
				t->curr++;
			}

			while (is_num_with_base(t->curr[0], base))
				t->curr++;

			// second check for float
			if (t->curr[0] == '.')
			{
				IsFloat = true;
				t->curr++;
			}

			while (is_num(t->curr[0]))
				t->curr++;

			if (IsFloat)
				token.type = TOKEN_FLOAT;

			token.len = t->curr - token.text;
		}
	} break;
	}

	return token;
}
