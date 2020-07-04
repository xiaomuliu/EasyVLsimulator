#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <assert.h>
#include "tokens.h"
#include "statements.h"
#include "wires.h"
#include "components.h"
#include "assign.h"
#include "syntax.h"

bool process_syntax(evl_statements &statements, evl_syntax &syntax)
{
	for (evl_statements::iterator it = statements.begin(); it != statements.end(); ++it) 
	{
		if (it->type == evl_statement::WIRE)
			process_wire_statement(syntax.wires, *it);
		else if (it->type == evl_statement::COMPONENT)
			process_component_statement(syntax.comps, *it, syntax.wires);
		else if (it->type == evl_statement::MODULE)
		{	
			it->tokens.pop_front();
			syntax.module.type = it->tokens.front().str;
		}
		else if (it->type == evl_statement::ASSIGN)
			process_assign_statement(syntax.comps, *it, syntax.wires);
	}
	
	/*
	statement_type_table type_counts;
	count_statement_by_types(type_counts, statements);
	syntax.module.num_wires = type_counts[evl_statement::WIRE];
	syntax.module.num_comps = type_counts[evl_statement::COMPONENT];
	*/

	syntax.module.num_wires = syntax.wires.size();
	syntax.module.num_comps = syntax.comps.size();

	return true;
}

void display_syntax(std::ostream &out, const evl_syntax &syntax)
{	
	//display module
	out << "module " << syntax.module.type << " " << syntax.module.num_wires
	<< " " << syntax.module.num_comps << std::endl;
	//display wires
	display_wires(out, syntax.wires);
	//display components
	display_components(out, syntax.comps);
}

bool store_syntax_to_file(std::string file_name, const evl_syntax &syntax) 
{
	std::ofstream output_file(file_name.c_str());
	// verify output_file is ready
	if (!output_file)
    {
        std::cerr << "Cannot create " << file_name << "!" << std::endl;
        return false;
    }

	display_syntax(output_file, syntax);
	return true;
}