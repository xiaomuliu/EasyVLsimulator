#ifndef GUARD_syntax_h
#define GUARD_syntax_h

#include <string>
#include "statements.h"
#include "wires.h"
#include "components.h"

struct evl_module
{
	std::string type;
	int num_wires;
	int num_comps;
}; //struct evl_module

struct evl_syntax
{
	evl_module module;
	evl_wires wires;
	evl_components comps;
}; //struct evl_syntax

bool process_syntax(evl_statements&, evl_syntax&);
void display_syntax(std::ostream&, const evl_syntax&);
bool store_syntax_to_file(std::string, const evl_syntax &); 

#endif

