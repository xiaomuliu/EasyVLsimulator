#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include "netlist.h"

//gate::~gate(){}
//netlist::~netlist(){}
/*
//destructor of gate
gate::~gate() {
	pins_.~vector();
}

//destructor of netlist
netlist::~netlist() {
 // delete pointers stored in gates_ and nets_
	gates_.~list();
	nets_.~map();
}
*/

gate::~gate(){
	for (size_t i = 0; i < pins_.size(); ++i)
		delete pins_[i];
}

netlist::~netlist(){
	for (std::list<gate *>::iterator i = gates_.begin();
		i != gates_.end(); ++i)
		delete *i;
	for (std::map<std::string, net *>::iterator i = nets_.begin();
		i != nets_.end(); ++i)
		delete i->second;
	nets_.clear();
}

std::string net::get_name() const{
	return net_name_;
}

size_t pin::get_pin_index() const{
	return pin_index_;
}

gate* pin::get_gate() const{
	return gate_;
}

std::string gate::get_type() const{
	return type_;
}

std::string gate::get_name() const{
	return name_;
}

//********************************************************************
//netlist creation
bool netlist::create(const evl_wires &wires, const evl_components &comps) 
{
	return create_nets(wires) && create_gates(comps);
}

//generate "wire_name[i]" as a string
std::string make_net_name(std::string wire_name, int i) {
	assert(i >= 0);
	std::ostringstream oss;
	oss << wire_name << "[" << i << "]";
	return oss.str();
}

bool netlist::create_nets(const evl_wires &wires) {
	for (evl_wires::const_iterator it = wires.begin();
		it != wires.end(); ++it){
		//it->first: the name of wire; it->second: width
		if (it->second == 1) {
			create_net(it->first);
		}
		else {
			for (int i = 0; i < it ->second; ++i) 
				create_net(make_net_name(it->first, i)); //wire[i]
		}
	}
	return true;
}

void netlist::create_net(std::string net_name) {
	assert(nets_.find(net_name) == nets_.end());
	nets_[net_name] = new net(net_name);
}

bool netlist::create_gates(const evl_components &comps) {
	for (evl_components::const_iterator it = comps.begin(); 
		it != comps.end(); ++it) {
		create_gate(*it);
	}
	return true;
}

bool netlist::create_gate(const evl_component &c) {
	//gates_.push_back(new gate(c.type, c.name));
	if (c.type == "dff") {
		gate *g = new flip_flop(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "and") {
		gate *g = new and_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "not") {
		gate *g = new not_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "buf") {
		gate *g = new buf(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "or") {
		gate *g = new or_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "xor") {
		gate *g = new xor_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "one") {
		gate *g = new one(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "zero") {
		gate *g = new zero(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "input") {
		gate *g = new input(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "output") {
		gate *g = new output(c.name);
		gates_.push_back(g);
	}

	return gates_.back()->create_pins(c.pins, nets_);
}

bool gate::create_pins(const evl_pins &pins,
	const std::map<std::string, net *> &netlist_nets) {
	size_t pin_index = 0; //position
	for (evl_pins::const_iterator p = pins.begin();
		p != pins.end(); ++p){
		create_pin(*p, pin_index, netlist_nets);
		++pin_index;
	}
	return validate_structural_semantics();
}

bool gate::create_pin(const evl_pin &p, size_t pin_index,
	const std::map<std::string, net *> &netlist_nets) {
	pins_.push_back(new pin);
	return pins_.back()->create(this, pin_index, p, netlist_nets);
}

bool pin::create(gate *g, size_t pin_index, const evl_pin &p,
	const std::map<std::string, net *> &netlist_nets) {
	//store g and pin_index;
	gate_ = g;
	pin_index_ = pin_index;

	if (p.bus_msb == -1) { // a 1-bit wire
		std::string net_name = p.name;
		//net = find net_name in netlist_nets;
		std::map<std::string, net *>::const_iterator it
			= netlist_nets.find(net_name);
		
		//nets_.push_back(net);
		//net.append_pin(this, 0);
		nets_.push_back(it->second); //pin::nets
		(*(it->second)).append_pin(this,0);
	}
	else { // a bus
		size_t net_index = 0;
		for (int i = p.bus_lsb; i <= p.bus_msb; ++i, ++net_index) {
			std::string net_name = make_net_name(p.name, i);
			//net = find net_name in netlist_nets
			std::map<std::string, net *>::const_iterator it
				= netlist_nets.find(net_name);

			//nets_.push_back(net);
			//net.append_pin(this, net_index);
			nets_.push_back(it->second);
			(*(it->second)).append_pin(this, net_index);
		}
	}
	return true;
}

void net::append_pin(pin *p, size_t net_index) {
	connections_.push_back(std::make_pair(p, net_index));
}

//***********************************************************************
//display and save
void net::display(std::ostream &out) const
{
	out << "net " << net_name_ << " " << 
		connections_.size() << std::endl;
	for (std::list<pin_info>::const_iterator pi = connections_.begin();
		pi != connections_.end(); ++pi)
	{	
		out << "pin " << (*((*((*pi).first)).get_gate())).get_type() << " " 
			<< (*((*((*pi).first)).get_gate())).get_name() << " " <<
			(*((*pi).first)).get_pin_index() << std::endl;
	}
}

void gate::display(std::ostream &out) const
{
	out << "gate " << type_ << " " << name_ 
		<< " " << pins_.size() << std::endl;
	for (std::vector<pin *>::const_iterator pi = pins_.begin();
		pi != pins_.end(); ++pi){
		out << "pin " << (**pi).nets_.size() << " ";
		for (std::vector<net *>::const_iterator ni = (**pi).nets_.begin();
			ni != (**pi).nets_.end(); ++ni){
			out << (**ni).get_name()<< " ";
		}
		out << std::endl;
	}
}

bool netlist::save(std::string &file)
{
	std::ofstream output_file(file.c_str());
	// verify output_file is ready
	if (!output_file)
    {
        std::cerr << "Cannot create " << file << "!" << std::endl;
        return false;
    }

	output_file << nets_.size() << " " << gates_.size() << std::endl;

	for (std::map<std::string, net *>::const_iterator i = nets_.begin();
		i != nets_.end(); ++i)
		(*(i->second)).display(output_file);
	
	for (std::list<gate *>::const_iterator j = gates_.begin();
		j != gates_.end(); ++j)
		(**j).display(output_file);
	
	return true; 
}

//********************************************************************
//validate semantics
//some parts left for project 4
bool flip_flop::validate_structural_semantics() {
	if (pins_.size() != 2) return false;
	//each pin should have a width of 1
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	//pins_[0]->set_as_output(); 
	//pins_[1]->set_as_input();
	return true;
}

bool not_gate::validate_structural_semantics() {
	if (pins_.size() != 2) return false;
	//each pin should have a width of 1
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	return true;
}

bool buf::validate_structural_semantics() {
	if (pins_.size() != 2) return false;
	//each pin should have a width of 1
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	return true;
}

bool and_gate::validate_structural_semantics() {
	if (pins_.size() < 3) return false;
	//each pin should have a width of 1
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}

	/*
	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i)
		pins_[i]->set_as_input();
	*/
	return true;
}

bool or_gate::validate_structural_semantics() {
	if (pins_.size() < 3) return false;
	//each pin should have a width of 1
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	return true;
}

bool xor_gate::validate_structural_semantics() {
	if (pins_.size() < 3) return false;
	//each pin should have a width of 1
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	return true;
}

bool one::validate_structural_semantics() {
	if (pins_.size() < 1) return false;
	return true;
}

bool zero::validate_structural_semantics() {
	if (pins_.size() < 1) return false;
	return true;
}

bool input::validate_structural_semantics() {
	if (pins_.size() < 1) return false;
	return true;
}

bool output::validate_structural_semantics() {
	if (pins_.size() < 1) return false;
	return true;
}