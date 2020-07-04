#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <assert.h>
#include <map>
#include "tokens.h"
#include "statements.h"

bool token_is_semicolon(const evl_token &token) 
{
	//return token.type == evl_token::SEMICOLON;
	return token.str == ";";
}

bool move_tokens_to_statement(evl_tokens &statement_tokens, evl_tokens &tokens) 
{
	assert(statement_tokens.empty());
	assert(!tokens.empty());
	
	evl_tokens::iterator next_sc = std::find_if(tokens.begin(), tokens.end(), token_is_semicolon);
	if (next_sc == tokens.end()) {
		std::cerr << "Look for ';' but reach the end of file" << std::endl;
		return false;
	}
	
	// move tokens within [tokens.begin(), next_sc] to statement_tokens
	++next_sc;
	statement_tokens.splice(statement_tokens.begin(), tokens, tokens.begin(), next_sc);

	return true;
}

bool group_tokens_into_statements(evl_statements &statements, evl_tokens &tokens)
{
	assert(statements.empty());
	for (; !tokens.empty();) { // generate one statement per iteration
		evl_token token = tokens.front();
		
		if (token.type != evl_token::NAME) {
			std::cerr << "Need a NAME token but found '" << token.str
			<< "' on line " << token.line_no << std::endl;
			return false;
		}

		if (token.str == "endmodule") { // ENDMODULE statement
			//token = tokens.front();
			//continue; // skip the rest for the current iteration
			break;
		}

		evl_statement statement;
		if (token.str == "module") { // MODULE statement
			statement.type = evl_statement::MODULE;
		}
		else if (token.str == "wire") { // WIRE statement
			statement.type = evl_statement::WIRE;
		}
		else if (token.str == "assign") { //ASSIGN statement
			statement.type = evl_statement::ASSIGN;
		}
		else{ // COMPONENT statement
			statement.type = evl_statement::COMPONENT;
		}

		if (!move_tokens_to_statement(statement.tokens, tokens))
			return false;
		assert(statement.tokens.back().str == ";");
		statements.push_back(statement);
	}
	return true;
}

//count_statement_by_types
/*
void count_statement_by_types(statement_type_table &type_counts, const evl_statements &statements)
{	
	for (evl_statements::const_iterator it = statements.begin(); it != statements.end(); ++it) 
	{
		++type_counts[it->type];
	}
	
	//for (statement_type_table::iterator map_it = type_counts.begin(); map_it != type_counts.end(); ++map_it)
	//{
	//	std::cout << "There are " << map_it->second <<
	//	" statements of type " << map_it->first << std::endl;
	//}

}
*/