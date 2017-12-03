
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


#include "Parser.h"

#define _SL_H_IMPLEMENTATION
#include "sl.h"


int main(int argc, char** argv)
{
	printf("Pine.exe: \n");

	if (argc < 2)
	{
		printf("Error: no input files specified.\n\n");
		printf("Usage: pine.exe file1.thsl [file2.thsl ...]\n");
		return 1;
	}

	for (int i = 1; i<argc; i++)
	{
		// TODO: Check the filename extension
		printf("%s\n", argv[i]);
		read_file_result fileContents = ReadEntireFile(argv[i]);

		if (fileContents.success)
		{
			Parser parser = {};
			parser.tokenizer = make_tokenizer(fileContents);
			Parse(&parser);
		}
	}

	return 0;
}