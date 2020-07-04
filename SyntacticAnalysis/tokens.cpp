#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <ctype.h>
#include "tokens.h"

//check if it's a single
bool is_a_single(char ch)
{
	return ((ch == '(') || (ch == ')') || (ch == '[') || (ch== ']')
		|| (ch == ':') || (ch == ';') || (ch == ','));
}

//check if it's a name
bool is_a_name(char ch)
{
	return (isalpha(ch) || (ch == '_') || (ch == '\\') || (ch == '.'));
}

//extract_tokens_from_line
bool extract_tokens_from_line(std::string line, int line_no, evl_tokens &tokens)
{ 
	for (size_t i = 0; i < line.size();)
	{
			// skip comments
			if (line[i] == '/') 
			{
				++i;
				if ((i == line.size()) || (line[i] != '/')) {
					std::cerr << "LINE " << line_no
						<< ": a single / is not allowed" << std::endl;
					return -1;
				}
				break; // skip the rest of the line by exiting the loop
			}

			// skip spaces
			else if (isspace(line[i]))
			{
				++i; // skip this space character
				continue; // skip the rest of the iteration
			}
			
			else if (is_a_single(line[i]))
			{
				evl_token token;
				token.type = evl_token::SINGLE;
				token.str = std::string(1, line[i]);
				token.line_no = line_no;
				tokens.push_back(token);

				++i; // we consumed this character
				continue; // skip the rest of the iteration
			}// a SINGLE token

			else if (is_a_name(line[i]))
			{
				size_t name_begin = i;
				for (++i; i < line.size(); ++i)
				{
					if (!(isalpha(line[i]) || (isdigit(line[i]))
						|| (line[i] == '_') || (line[i] == '\\') || (line[i] == '.')))
					{
						break; // [name_begin, i) is the range for the token
					}
				}

				evl_token token;
				token.type = evl_token::NAME;
				token.str = std::string(line.substr(name_begin, i-name_begin));
				token.line_no = line_no;
				tokens.push_back(token);

			} // a NAME token

			else if (isdigit(line[i]))
			{
				size_t num_begin = i;
				for (++i; i < line.size() && isdigit(line[i]); ++i){}
				
				evl_token token;
				token.type = evl_token::NUMBER;
				token.str = std::string(line.substr(num_begin, i-num_begin));
				token.line_no = line_no;
				tokens.push_back(token);

			}// a NUMBER token

			else
			{
				std::cerr << "LINE " << line_no
				<< ": invalid character" << std::endl;
				return false;
			}
	}
	return true; 
}

//extract_tokens_from_file
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

//display_tokens
/*
void display_tokens(std::ostream &out, const evl_tokens &tokens) 
{
	for (size_t i = 0; i < tokens.size(); ++i)
	{
		if (tokens[i].type == evl_token::SINGLE)
		{
			out << "SINGLE " << tokens[i].str << std::endl;
		}
		
		else if (tokens[i].type == evl_token::NAME)
		{
			out << "NAME " << tokens[i].str << std::endl;
		}

		else 
		{
			out << "NUMBER " << tokens[i].str << std::endl;
		}
	}
}

//store_tokens_to_file
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

//count_tokens_by_types
void count_tokens_by_types(const evl_tokens &tokens)
{
	typedef std::map<evl_token::token_type, int> token_type_table;
	token_type_table type_counts;

	for (evl_tokens::const_iterator it = tokens.begin(); it != tokens.end(); ++it) 
	{
		++type_counts[it->type];
	}

	for (token_type_table::iterator map_it = type_counts.begin(); map_it != type_counts.end(); ++map_it)
	{
		std::cout << "There are " << map_it->second <<
		" tokens of type " << map_it->first << std::endl;
	}
}
*/