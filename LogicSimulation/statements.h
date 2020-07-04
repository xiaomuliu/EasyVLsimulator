#ifndef GUARD_statements_h
#define GUARD_statements_h

#include <vector>
#include <list>
#include <map>
#include "tokens.h"

struct evl_statement
{
	enum statement_type { MODULE, WIRE, COMPONENT, ENDMODULE};
	statement_type type;
	evl_tokens tokens;
}; // struct evl_statement

typedef std::list<evl_statement> evl_statements;
//typedef std::map<evl_statement::statement_type, int> statement_type_table;

bool group_tokens_into_statements(evl_statements &, evl_tokens&);
//void count_statement_by_types(statement_type_table &type_counts, const evl_statements&);

#endif