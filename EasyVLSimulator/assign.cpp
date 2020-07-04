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
#include "assign.h"

bool process_assign_statement(evl_components &comps, evl_statement &s, evl_wires &wires)
{
	assert(s.type == evl_statement::ASSIGN);
	enum state_type {INIT, LHS, LHS_NAME, LHS_BUS, LHS_MSB, LHS_COLON, LHS_LSB, LHS_DONE,
		RHS, RHS_NAME, RHS_BUS, RHS_MSB, RHS_COLON, RHS_LSB, RHS_DONE, DONE};
	state_type state = INIT;

	evl_assign assign;
	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front()) {
		//pop out a token from tokens each time
		evl_token t = s.tokens.front();
		if (state == INIT) {
			if (t.str == "assign") {
			state = LHS;
			}
			else {
			std::cerr << "Need 'assign' but found '" << t.str
			<< "' on line " << t.line_no << std::endl;
			return false;
			}
		}

		else if (state == LHS) {
			if (t.type == evl_token::NAME) {
				assign.lhs.name = t.str;
				assign.lhs.bus_msb = -1;
				assign.lhs.bus_lsb = -1;
				state = LHS_NAME;
			}
			else{
				std::cerr << "Need a NAME token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == LHS_NAME) {
			if (t.str == "["){
				state = LHS_BUS;
			}
			else if (t.str == "="){
				if(wires[assign.lhs.name] > 1){// wire is a bus
					//update lsb and msb
					assign.lhs.bus_lsb = 0; 
					assign.lhs.bus_msb = wires[assign.lhs.name]-1;
				}
				state = RHS;
			}
			else{
				std::cerr << "Need '[' or '=' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == LHS_BUS) {
			if (t.type == evl_token::NUMBER) {
				assign.lhs.bus_msb = atoi(t.str.c_str());
				state = LHS_MSB;
			}
			else{
				std::cerr << "Need a NUMBER token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == LHS_MSB) {
			if (t.str == ":"){
				state = LHS_COLON;
			}
			else if (t.str == "]"){
				//lsb should be updated to the same as the msb
				assign.lhs.bus_lsb = assign.lhs.bus_msb; 
				state = LHS_DONE;
			}
			else{
				std::cerr << "Need ':' or ']' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == LHS_COLON) {
			if (t.type == evl_token::NUMBER) {
				assign.lhs.bus_lsb = atoi(t.str.c_str());
				state = LHS_LSB;
			}
			else{
				std::cerr << "Need a NUMBER token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == LHS_LSB) {
			if (t.str == "]"){
				state = LHS_DONE;
			}
			else{
				std::cerr << "Need ']' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == LHS_DONE){
			if (t.str == "="){
				state = RHS;
			}
			else{
				std::cerr << "Need '=' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		//*******************RHS******************************
		else if (state == RHS) {
			if (t.type == evl_token::NAME) {
				assign.rhs.name = t.str;
				assign.rhs.bus_msb = -1;
				assign.rhs.bus_lsb = -1;
				state = RHS_NAME;
			}
			else{
				std::cerr << "Need a NAME token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == RHS_NAME) {
			if (t.str == "["){
				state = RHS_BUS;
			}
			else if (t.str == ";"){
				if(wires[assign.rhs.name] > 1){// wire is a bus
					//update lsb and msb
					assign.rhs.bus_lsb = 0; 
					assign.rhs.bus_msb = wires[assign.rhs.name]-1;
				}
				state = DONE;
			}
			else{
				std::cerr << "Need '[' or ';' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == RHS_BUS) {
			if (t.type == evl_token::NUMBER) {
				assign.rhs.bus_msb = atoi(t.str.c_str());
				state = RHS_MSB;
			}
			else{
				std::cerr << "Need a NUMBER token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == RHS_MSB) {
			if (t.str == ":"){
				state = RHS_COLON;
			}
			else if (t.str == "]"){
				//lsb should be updated to the same as the msb
				assign.rhs.bus_lsb = assign.rhs.bus_msb; 
				state = RHS_DONE;
			}
			else{
				std::cerr << "Need ':' or ']' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == RHS_COLON) {
			if (t.type == evl_token::NUMBER) {
				assign.rhs.bus_lsb = atoi(t.str.c_str());
				state = RHS_LSB;
			}
			else{
				std::cerr << "Need a NUMBER token but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == RHS_LSB) {
			if (t.str == "]"){
				state = RHS_DONE;
			}
			else{
				std::cerr << "Need ']' but found '" << t.str
				<< "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == RHS_DONE){
			if (t.str == ";"){
				state = DONE;
			}
			else{
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
		std::cerr << "something wrong with the statement" << std::endl;
		return false;
	}

	//for each "assign", add one/multiple buf gate(s).
	else if ((assign.lhs.bus_msb-assign.lhs.bus_lsb) == 0){ 
		//a wire, both msb and lsb are -1 or a pin with width = 1
		evl_component comp;
		comp.type = "buf"; comp.name = "assign";
		comp.pins.push_back(assign.lhs);
		comp.pins.push_back(assign.rhs);

		comps.push_back(comp);
	}
	else{
		for (int i = 0; i < (assign.lhs.bus_msb-assign.lhs.bus_lsb+1); ++i)
		{
			evl_component comp;
			comp.type = "buf"; comp.name = "assign";
			evl_pin temp_pin;
			temp_pin.name = assign.lhs.name;
			temp_pin.bus_lsb = assign.lhs.bus_lsb+i;
			temp_pin.bus_msb = temp_pin.bus_lsb;
			comp.pins.push_back(temp_pin);//output(lhs)
			
			temp_pin.name = assign.rhs.name;
			temp_pin.bus_lsb = assign.rhs.bus_lsb+i;
			temp_pin.bus_msb = temp_pin.bus_lsb;
			comp.pins.push_back(temp_pin);//intput(rhs)

			comps.push_back(comp);
		}
	}

	return true;
}




