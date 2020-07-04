#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <assert.h>
#include "tokens.h"
#include "statements.h"
#include "wires.h"

bool process_wire_statement(evl_wires &wires, evl_statement &s) 
{
	assert(s.type == evl_statement::WIRE);
	enum state_type {INIT, WIRE, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, WIRES, WIRE_NAME, DONE};
	state_type state = INIT;

	int bus_width = 1;
	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front()) {
		//pop out a token from tokens each time
		evl_token t = s.tokens.front();
		if (state == INIT) {
			if (t.str == "wire") {
			state = WIRE;
			}
			else {
			std::cerr << "Need 'wire' but found '" << t.str
			<< "' on line " << t.line_no << std::endl;
			return false;
			}
		}

		else if (state == WIRE) {
			if (t.type == evl_token::NAME) {
				evl_wires::iterator it = wires.find(t.str);
				if (it != wires.end()) {
					std::cerr << "Wire '" << t.str << "' on line "
						<< t.line_no << "is already defined" << std::endl;
					return false;
				}
				wires[t.str] = bus_width;
				state = WIRE_NAME;
			}
			else if (t.str == "["){
				state = BUS;
			}
			else{
				std::cerr << "Need '[' or wire name but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS){
			if (t.type == evl_token::NUMBER)
			{
				//update it width
				bus_width = atoi(t.str.c_str()) + 1;
				state = BUS_MSB;
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
			else{
				std::cerr << "Need ':' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS_COLON){
			if (t.str == "0"){
				state = BUS_LSB;
			}
			else{
				std::cerr << "Need '0' but found '" << t.str
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
			if (t.type == evl_token::NAME) {
				evl_wires::iterator it = wires.find(t.str);
				if (it != wires.end()) {
					std::cerr << "Wire '" << t.str << "' on line "
					<< t.line_no << "is already defined" << std::endl;
					return false;
				}
				wires[t.str] = bus_width;
				state = WIRE_NAME;
			}
		}

		else if (state == WIRES) {
			if (t.type == evl_token::NAME) {
				evl_wires::iterator it = wires.find(t.str);
				if (it != wires.end()) {
					std::cerr << "Wire '" << t.str << "' on line "
					<< t.line_no << "is already defined" << std::endl;
					return false;
				}
				wires[t.str] = bus_width;
				state = WIRE_NAME;
			}
		} // same as the branch for WIRE

		else if (state == WIRE_NAME) {
			if (t.str == ",") {
				state = WIRES;
			}
			else if (t.str == ";") {
				state = DONE;
			}
			else {
				std::cerr << "Need ',' or ';' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}
	
		else {
			assert(false); // shouldn't reach here
		} 
	}
	
	if (!s.tokens.empty() || (state != DONE)) {
		std::cerr << "something wrong with the statement" << std::endl;
		return false;
	}
	return true;
}

void display_wires(std::ostream &out, const evl_wires &wires)
{
	for(evl_wires::const_iterator it = wires.begin(); it != wires.end(); ++it)
	{
		out << "wire " << it->first << " " << it->second << std::endl;
	}
}