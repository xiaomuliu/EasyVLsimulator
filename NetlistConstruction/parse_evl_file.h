#ifndef GUARD_parse_evl_file_h
#define GUARD_parse_evl_file_h

#include <iostream>
#include <fstream>
#include <string>
#include "tokens.h"
#include "statements.h"
#include "wires.h"
#include "components.h"
#include "syntax.h"

bool parse_evl_file(std::string file_name, evl_wires &wires, evl_components &comps)
{
	// extract tokens
	evl_tokens tokens;
	if (!extract_tokens_from_file(file_name, tokens)) 
		return false;

	//group tokens into statements
	evl_statements statements;
	if (!group_tokens_into_statements(statements, tokens))
		return false;
	
	//syntactic Analysis
	evl_syntax syntax;
	if (!process_syntax(statements, syntax))
		return false;
	wires = syntax.wires;
	comps = syntax.comps;

	//display_syntax(std::cout, syntax);
	return true;
}

#endif