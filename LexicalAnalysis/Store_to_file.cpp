#include <iostream>
#include <fstream>
#include "tokens.h"
#include "Display_tokens.h"

bool store_tokens_to_file(std::string file_name, const evl_tokens &tokens) 
{
	std::ofstream output_file(file_name.c_str());
	// verify output_file is ready
	if (!output_file)
    {
        std::cerr << "Cannot create " << file_name << "!" << std::endl;
        return -1;
    }

	display_tokens(output_file, tokens);
	return true;
}