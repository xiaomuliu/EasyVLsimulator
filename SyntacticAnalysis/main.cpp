#include <iostream>
#include <fstream>
#include <string>
#include "tokens.h"
#include "statements.h"
#include "wires.h"
#include "components.h"
#include "syntax.h"

int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
        std::cerr << "You should provide a file name." << std::endl;
        return -1;
    }// verify that argv[1] exists

	// extract tokens
	evl_tokens tokens;
	if (!extract_tokens_from_file(argv[1], tokens)) 
		return -1;

	//group tokens into statements
	evl_statements statements;
	if (!group_tokens_into_statements(statements, tokens))
		return -1;
	
	//syntactic Analysis
	evl_syntax syntax;
	if (!process_syntax(statements, syntax))
		return -1;

	//display_syntax(std::cout, syntax);

	//save syntax into a file
	std::string out_name = argv[1];
	out_name += ".syntax";
	if (!store_syntax_to_file(out_name, syntax))
		return -1;
	else
	{	
		std::cout << "Syntax saved to " << out_name << std::endl;
		return 0;
	}
	
}