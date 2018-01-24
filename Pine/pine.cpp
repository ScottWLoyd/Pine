/*

	Language:
		strict typed
		functions, no closures or nested functions
		end statements with semicolon
		blocks with {}
		if, while, for
		operators: + - * / % & | ^ ~ !
		comparators: 
			== 
			!= 
		structs, unions

*/
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

#define internal static

void error(char* format, ...)
{
	char message[1024];
	va_list args;
	va_start(args, format);
	vsprintf_s(message, format, args);
	va_end(args);
	printf(message);
}

enum token_t {
	TOKEN_IDENT = 128,
	TOKEN_INT,
	TOKEN_REAL, 
	TOKEN_EQUALS,
	TOKEN_IF,
	TOKEN_WHILE,
	TOKEN_FOR,
	TOKEN_RETURN,
};

struct token
{
	token_t type;
	union
	{
		u64 int_val;
		f64 real_val;
		char* str_val;
	};
	// TODO(scott): line, column number
};

internal char* token_to_identifier(token tok)
{
	char buffer[128];
	if (tok.type < 128)
	{
		sprintf_s(buffer, sizeof(buffer), "%c", tok.type);
	}
	else if (tok.type == TOKEN_INT)
	{
		sprintf_s(buffer, sizeof(buffer), "%d", tok.int_val);
	}
	else if (tok.type == TOKEN_REAL)
	{
		sprintf_s(buffer, sizeof(buffer), "%f", tok.real_val);
	}
	else if (tok.type == TOKEN_IDENT)
	{
		sprintf_s(buffer, sizeof(buffer), "%s", tok.str_val);
	}
	else
	{
		assert(false);
	}
	return buffer;
}

internal char* parse_ptr;
internal u32 line_no;
internal token curr_tok;

enum
{
	MAX_SYMBOLS = 256,
};
internal char* symbols[MAX_SYMBOLS];
internal u32 num_symbols;

internal char* intern(char* start, char* end)
{
	for (int i = 0; i < num_symbols; i++)
	{
		char* symbol = symbols[i];
		char* string = start;
		while (*symbol && string != end && *symbol == *string)
		{
			symbol++;
			string++;
		}
		if (*symbol == 0 && string == end)
		{
			return symbols[i];
		}
	}
	// didn't find the symbol - add it if we can
	size_t ident_len = end - start;
	char* new_symbol = (char*)malloc(ident_len + 1);
	memcpy(new_symbol, start, ident_len);
	new_symbol[ident_len] = 0;
	if (num_symbols == MAX_SYMBOLS)
	{
		error("Unable to declare variable '%s'. Maximum number of symbols reached already!\n", new_symbol);
	}
	symbols[num_symbols++] = new_symbol;
	return new_symbol;
}

internal void next_token()
{
	while (isspace(*parse_ptr))
	{
		if (*parse_ptr == '\n')
			line_no++;
		parse_ptr++;
	}
		
	// Numbers:
	//		integer: [0-9]+
	//		real: [0-9]+[.][0-9]*
	if (isdigit(*parse_ptr))
	{
		char* start_num = parse_ptr;
		bool is_real = false;
		parse_ptr++;
		while (isdigit(*parse_ptr) || *parse_ptr == '.')
		{
			if (*parse_ptr == '.')
			{
				if (is_real)
				{
					error("Parse error at line number %d: extra '.' found in real literal\n", line_no);
				}
				else
				{
					is_real = true;
				}
			}
			parse_ptr++;
		}

		size_t token_len = parse_ptr - start_num;
		if (token_len > 64)
		{
			error("Parse error at line number %d: numeric literal is larger than 64 digits!\n", line_no);
		}

		if (is_real)	
		{
			curr_tok.type = TOKEN_REAL;
			char* num_string = (char*)malloc(token_len + 1);
			for (int i = 0; i < token_len; i++)
				num_string[i] = start_num[i];
			num_string[token_len] = 0;
			curr_tok.real_val = atof(num_string);
			free(num_string);
		}
		else
		{
			curr_tok.type = TOKEN_INT;
			curr_tok.int_val = 0;
			while (start_num < parse_ptr)
			{
				curr_tok.int_val = curr_tok.int_val*10 + (*start_num++ - '0');
			}
		}
	}
	// Identifiers: [a-zA-Z_][a-zA-Z0-9_]*
	else if (isalpha(*parse_ptr) || *parse_ptr == '_')
	{
		char* token_start = parse_ptr;
		parse_ptr++;
		while (isalnum(*parse_ptr) || *parse_ptr == '_')
			parse_ptr++;

		curr_tok.type = TOKEN_IDENT;
		curr_tok.str_val = intern(token_start, parse_ptr);
	}
	else
	{
		switch (*parse_ptr)
		{
			case '+':
			case '-':
			case '*':
			case '/':
			case '%':
			case '&':
			case '|':
			case '^':
			case '~':
			case '!':
			case '(':
			case ')':
			case '{':
			case '}': 
			case '=': {
				if (parse_ptr[1] == '=')
				{
					curr_tok.type = TOKEN_EQUALS;
					parse_ptr += 2; 
					break;
				}
			}
			case ';': {
				curr_tok.type = (token_t)*parse_ptr++;
			} break;
			default: {
				error("Parse error at line %d: unexpected token '%c'", line_no, *parse_ptr);
			} break;
		}
		
	}
}

internal char* keyword_if;
internal char* keyword_while;
internal char* keyword_for;

internal void initialize()
{
	char* string_if = "if";
	char* string_while = "while";
	char* string_for = "for";

	keyword_if = intern(string_if, string_if + sizeof(string_if) - 1);
	keyword_if = intern(string_while, string_while + sizeof(string_while) - 1);
	keyword_if = intern(string_for, string_for + sizeof(string_for) - 1);


	line_no = 0;
}

enum opcode_t
{
	LIT = 1,
	GET,
	SET,
	POP,
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	NEG,
	RET,
};

internal char* emit_pointer;

internal void emit(u8 byte)
{
	*emit_pointer++ = byte;
}

internal void emit_num(size_t data)
{
	*(size_t*)emit_pointer = data;
	emit_pointer += sizeof(size_t);
}

internal char* ip;
internal int* sp;
internal int* fp;

internal int evaluate()
{
	for (;;)
	{
		int op = *ip++;
		switch (op)
		{
			case LIT: {
				*sp++ = *(size_t*)ip;
				ip += sizeof(size_t);
			} break;
			case GET: {
				*sp++ = fp[*ip];
				ip++;
			} break;
			case SET: {
				fp[*ip] = sp[-1];
				ip++;
				sp--;
			} break;
			case POP: {
				sp--;
			} break;
			case ADD: {
				sp[-2] += sp[-1];
				sp--;
			} break;
			case SUB: {
				sp[-2] -= sp[-1];
				sp--;
			} break;
			case MUL: {
				sp[-2] *= sp[-1];
				sp--;
			} break;
			case DIV: {
				sp[-2] /= sp[-1];
				sp--;
			} break;
			case MOD: {
				sp[-2] %= sp[-1];
				sp--;
			} break;
			case NEG: {
				sp[-1] = -sp[-1];
			} break;
			case RET: {
				sp--;
				return *sp;
			} break;
		}
	}
}

internal void expect_token(token_t expected_token)
{
	if (curr_tok.type != expected_token)
	{
		error("Parse error: expected token '%s', found '%s'\n", token_to_identifier(curr_tok));
	}
	next_token();
}

internal int find_variable(char* ident)
{
	for (size_t i = 0; i < num_symbols; i++)
	{
		if (symbols[i] == ident)
		{
			return i;
		}
	}
	error("Internal compiler error: did not find symbol \"%s\" in lookup table.\n", ident);
	return -1;
}

internal void expr();

// expr2 = int | real | ident | '(' expr ')' | '-' expr2
internal void expr2()
{
	if (curr_tok.type == TOKEN_INT)
	{
		next_token();
		emit(LIT);
		emit_num(curr_tok.int_val);
	}
	else if (curr_tok.type == TOKEN_REAL)
	{
		next_token();
		emit(LIT);
		emit_num(curr_tok.real_val);
	}
	else if (curr_tok.type == TOKEN_IDENT)
	{
		int slot = find_variable(curr_tok.str_val);
		next_token();
		if (curr_tok.type == '=')
		{
			next_token();
			expr();
			emit(SET);
			emit(slot);
		}
		emit(GET);
		emit(slot);
	}
	else if (curr_tok.type == '(')
	{
		next_token();
		expr();
		expect_token((token_t)')');
	} 
	else if (curr_tok.type == '-')
	{
		next_token();
		expr2();
		emit(NEG);
	}
	else
	{
		error("Unexpected token '%s' at beginning of expression.", token_to_identifier(curr_tok));
	}
}

// expr1 = expr2 { '*' expr2 | '/' expr2 | '%' expr2 }
internal void expr1()
{
	expr2();
	while (curr_tok.type == '*' || curr_tok.type == '/' || curr_tok.type == '%')
	{
		token_t op = curr_tok.type;
		next_token();
		expr2();
		if (op == '*')
		{
			emit(MUL);
		}
		else if (op == '/')
		{
			emit(DIV);
		}
		else 
		{
			emit(MOD);
		}
	}
}

// expr = expr1 { '+' expr1 | '-' expr1 }
internal void expr()
{
	expr1();
	while (curr_tok.type == '+' || curr_tok.type == '-')
	{
		token_t op = curr_tok.type;
		next_token();
		expr1();
		if (op == '+')
		{
			emit(ADD);
		}
		else 
		{
			emit(SUB);
		}
	}
}


internal void block()
{
	expr();
	while (curr_tok.type == ';')
	{
		next_token();
		emit(POP);
		expr();
	}
	emit(RET);
}

internal void stmt();

internal void stmtblock()
{
	expect_token((token_t)'{');
	// TODO(scott): investigate this.
	while (curr_tok.type && curr_tok.type != '}')
	{
		stmt();
	}
	expect_token((token_t)'}');
}

// stmtlist = stmt 
//			| '{' stmt { stmtlist } '}'
// stmt = identifier '=' expr 
//		| 'if' expr stmtlist { 'elsif' expr stmtlist } { else stmtlist }
//		| 'while' expr stmtlist
internal void stmt()
{
	if (curr_tok.type == TOKEN_IF)
	{
		next_token();
		expr();
		stmtblock();
	}
	else if (curr_tok.type == TOKEN_WHILE)
	{
		next_token();
		expr();
		stmtblock();
	}
	else if (curr_tok.type == TOKEN_RETURN)
	{
		next_token();
		expr();
		expect_token((token_t)';');
		emit(RET);
	}
	else if (curr_tok.type == '{')
	{
		stmtblock();
	}
	else
	{
		expr();
		emit(POP);
		expect_token((token_t)';');
	}
}

int main(int ArgCount, char** Args)
{
	char emit_buffer[1024];
	emit_pointer = emit_buffer;

	initialize();
	char* code = "1*2+3*4";
	parse_ptr = code;

	next_token();
	block();

	int frame[1024];
	int stack[1024];

	ip = emit_buffer;
	fp = frame;
	sp = stack;

	int result = evaluate();

	return 0;
}