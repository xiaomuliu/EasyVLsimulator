#include <iostream>
#include "tokens.h"

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