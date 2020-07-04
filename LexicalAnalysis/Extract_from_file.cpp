#include <iostream>
#include <fstream>
#include <string>
#include "tokens.h"
#include "Extract_from_line.h"

bool extract_tokens_from_file(std::string file_name, evl_tokens &tokens) 
{ 
	std::ifstream input_file(file_name.c_str());
	if (!input_file) {
		std::cerr << "Can't read " << file_name << "!" << std::endl;
		return false;
	}
	// extract tokens from each line
	tokens.clear(); 
	std::string line;
	for (int line_no = 1; std::getline(input_file, line); ++line_no) {
		if (!extract_tokens_from_line(line, line_no, tokens)) {
			return false;
		}
	}
	return true;
}