#ifndef GUARD_components_h
#define GUARD_components_h

#include <string>
#include <list>
#include "statements.h"
#include "wires.h"

struct evl_pin
{
	std::string name;
	int bus_msb;
	int bus_lsb;
}; //struct evl_pin
typedef std::list<evl_pin> evl_pins;

struct evl_component
{
	std::string type;
	std::string name;
	std::list<evl_pin> pins;
}; // struct evl_component
typedef std::list<evl_component> evl_components;

bool process_component_statement(evl_components&, evl_statement&, evl_wires&);
void display_components(std::ostream&, const evl_components&);

#endif