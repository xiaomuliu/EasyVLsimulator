#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <assert.h>
#include "tokens.h"
#include "statements.h"
#include "wires.h"
#include "components.h"

bool process_component_statement(evl_components &comps, evl_statement &s, evl_wires &wires) 
{
	assert(s.type == evl_statement::COMPONENT);
	enum state_type {INIT, TYPE, NAME, PINS, PIN_NAME, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, PINS_DONE, DONE};
	state_type state = INIT;

	evl_component comp;
	evl_pin pin;
	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front()) {
		//pop out a token from tokens each time
		evl_token t = s.tokens.front();
		if (state == INIT) {
			if (t.type == evl_token::NAME){				
				comp.type = t.str; comp.name = "NONE";
				state = TYPE;
			}
			else {
				std::cerr << "Need component type but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == TYPE) {
			if (t.str == "("){
				state = PINS;
			}
			else if (t.type == evl_token::NAME) {
				comp.name = t.str;
				state = NAME;
			}
			else{
				std::cerr << "Need a NAME token or '(' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == NAME){
			if (t.str == "("){
				state = PINS;
			}
			else{
				std::cerr << "Need '(' token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == PINS){
			if (t.type == evl_token::NAME){
				// The name of a pin must be the name of some wires
				evl_wires::iterator it = wires.find(t.str);
				if (it == wires.end()) {
					std::cerr << "Pin '" << t.str << "' on line "
					<< t.line_no << "is not the name of a wire defined" << std::endl;
					return false;
				}
				else{
					pin.name = t.str;
					pin.bus_msb = -1;
					pin.bus_lsb = -1;

					state = PIN_NAME;
				}
			}
			else{
				std::cerr << "Need a NAME token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == PIN_NAME){
			if (t.str == ","){
				if(wires[pin.name] > 1){// wire is a bus
					pin.bus_lsb = 0; pin.bus_msb = wires[pin.name]-1;
				}
				comp.pins.push_back(pin);
				state = PINS;
			}
			else if (t.str == ")"){
				if(wires[pin.name] > 1){// wire is a bus
					pin.bus_lsb = 0; pin.bus_msb = wires[pin.name]-1;
				}
				comp.pins.push_back(pin);
				state = PINS_DONE;
			}
			else if (t.str == "["){
				state = BUS;
			}
			else{
				std::cerr << "Need ',', ') or '[' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS){
			if (t.type == evl_token::NUMBER)
			{
				pin.bus_msb = atoi(t.str.c_str());
				if (pin.bus_msb < wires[pin.name] && pin.bus_msb >= 0)
				{ 
					state = BUS_MSB;
				}
				else if(wires[pin.name] > 1 && pin.bus_msb < 0) //wire is a bus
				{
					std::cerr << "Pin " << pin.name <<" msb less than zero on line " 
					<< t.line_no << std::endl;
					return false;
				}
				else
				{
					std::cerr << "Pin " << pin.name <<" msb exceeds the wire width on line " 
					<< t.line_no << std::endl;
					return false;
				}
			}
			else{
				std::cerr << "Need a NUMBER token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS_MSB){
			if (t.str == ":"){
				state = BUS_COLON;
			}
			else if(t.str == "]"){
				pin.bus_lsb = pin.bus_msb;
				state = BUS_DONE;
			}
			else{
				std::cerr << "Need ':' or ']' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS_COLON){
			if (t.type == evl_token::NUMBER)
			{
				pin.bus_lsb = atoi(t.str.c_str());
				if (wires[pin.name] > 1 && pin.bus_lsb < 0) //wire is a bus
				{
					std::cerr << "Pin " << pin.name << " lsb less than zero"
					<< " on line " << t.line_no << std::endl;
					return false;

				}
				else if (pin.bus_lsb > pin.bus_msb)
				{
					std::cerr << "Pin " << pin.name << " lsb larger than msb "
					<< " on line " << t.line_no << std::endl;
					return false;
				}
				else
				{
					state = BUS_LSB;
				}
			}
			else{
				std::cerr << "Need a NUMBER token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS_LSB){
			if (t.str == "]"){
				state = BUS_DONE;
			}
			else{
				std::cerr << "Need ']' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS_DONE){
			if (t.str == ",") {
				comp.pins.push_back(pin);
				state = PINS;
			}
			else if (t.str == ")"){
				comp.pins.push_back(pin);
				state = PINS_DONE;
			}
			else{
				std::cerr << "Need ',' or ')' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == PINS_DONE) {
			if (t.str == ";") {
				state = DONE;
			}
			else {
				std::cerr << "Need ';' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}
	
		else {
			assert(false); // shouldn't reach here
		} 

	}

	if (!s.tokens.empty() || (state != DONE)) {
		std::cerr << "Something wrong with the statement" << std::endl;
		return false;
	}
	else{
		comps.push_back(comp);
		return true;
	}
}

//display
void display_components(std::ostream &out, const evl_components &components)
{
	for(evl_components::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		out << "component " << it->type << " " << it->name << " " << it->pins.size() << std::endl;
		//display pins
		for(std::list<evl_pin>::const_iterator i = it->pins.begin(); i != it->pins.end(); ++i)
		{	
			out << "pin " << i->name << " " << i->bus_msb << " " << i->bus_lsb << std::endl;
		}
	}
}