#include <iostream>
#include <vector>
#include <string>
#include "tokens.h"
#include "check.h"

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
				tokens.push_back(token);

			} // a NAME token

			else if (isdigit(line[i]))
			{
				size_t num_begin = i;
				for (++i; i < line.size() && isdigit(line[i]); ++i){}
				
				evl_token token;
				token.type = evl_token::NUMBER;
				token.str = std::string(line.substr(num_begin, i-num_begin));
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