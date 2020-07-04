#ifndef GUARD_assign_h
#define GUARD_assign_h

#include <string>
#include <list>
#include "statements.h"
#include "wires.h"
#include "components.h"

struct evl_assign
{
	evl_pin lhs;
	evl_pin rhs;
}; //struct evl_assign
typedef std::list<evl_assign> evl_assigns;

bool process_assign_statement(evl_components&, evl_statement&, evl_wires&);

#endif

