#ifndef GUARD_wires_h
#define GUARD_wires_h

#include <string>
#include <map>
#include "statements.h"

typedef std::map<std::string, int> evl_wires;

bool process_wire_statement(evl_wires&, evl_statement&);
void display_wires(std::ostream&, const evl_wires&);

#endif