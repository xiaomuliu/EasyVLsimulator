#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <assert.h>
#include <cmath>
#include "netlist.h"
#include "str_process.h"

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

std::vector<net *> pin::get_nets() const{
	return nets_;
}

net* pin::get_net(const size_t i) const{
	return nets_[i];
}//access the ith net connected to the pin

int pin::get_io_flag() const{
	return io_flag;
}

void pin::set_as_input() {
	io_flag = 1;
}

void pin::set_as_output() {
	io_flag = 0;
}

bool pin::isinput() const{
	if(get_io_flag() == 1)
		return true;
	else
		return false;
}

bool pin::isoutput() const{
	if(get_io_flag() == 0)
		return true;
	else
		return false;
}

std::vector<std::string> input::get_pin_values()
{
	return pin_values;
}

/*
void net::set_logic_value(bool value){
	logic_value = std::make_pair("Y",value);
}
*/

bool net::retrieve_logic_value() {
	if(logic_value.first != "?"){
		return logic_value.second;
	}
	else{
		logic_value.second = retrieve_from_driver();
		logic_value = std::make_pair("Y",logic_value.second);
		return logic_value.second;
	}
}

bool net::retrieve_from_driver() {
	for(std::list<pin_info>::const_iterator i = connections_.begin();
		i != connections_.end(); ++i){
		if(i->first->isoutput()){//the net should connect to an output pin
			gate* g = i->first->get_gate(); //the gate which the pin belongs to
			if(g->get_type() == "dff"){
				return g->get_state();
			}//flip flop: provides the current state bit
			
			else if(g->get_type() == "input"){
				std::vector<std::string> pin_values = g->get_pin_values();
				//according to the pin's position on the gate, return corresponding value
				std::string pin_value = pin_values[i->first->get_pin_index()];
				
				std::vector<net *> nets_ = i->first->get_nets();
				
				for(size_t i = 0; i != nets_.size(); ++i){
					//find corresponding bit
					if(nets_[i]->get_name() == this->get_name()){			
						if(pin_value[nets_.size()-1-i] == '1'){ return true; }
						else{ return false; }
					}
				}
			}//input gate: provides the values from the file
			
			else if(g->get_type() == "one"){
				return true; 
			}//one gate: provides 1
			else if(g->get_type() == "zero"){
				return false; 
			}//zero gate: provides 0
			else{
				return g->compute_output(); 
			}//other logic gates
		}
	}
}

/*
bool gate::retrieve_logic_value() {
	if(type_ == "dff"){
		return this->get_state();
	}//flip flop: provides the current state bit
	else if(type_ == "input"){
		return this->read_input();
	}//input gate: provides the values from the file
	else if(type_ == "one"){
		
		//for(size_t i = 0; i != this->pins_.size(); ++i){
		//	for(size_t j = 0; j != pins_[i]->nets_.size(); ++j){
		//		pins_[i]->nets_[j]->set_logic_value(1);
		//	}
		//}
		
		//this->set_output(); //one::set_output()
		return true; 
	}//one gate: provides 1
	else if(type_ == "zero"){
		
		//for(size_t i = 0; i != this->pins_.size(); ++i){
		//	for(size_t j = 0; j != pins_[i]->nets_.size(); ++j){
		//		pins_[i]->nets_[j]->set_logic_value(0);
		//	}
		//}
		
		//this->set_output(); //zero::set_output()
		return false; 
	}//zero gate: provides 0
	else{
		return this->compute_output(); 
	}//logic gates
}
*/

//********************************************************************
//netlist creation
bool netlist::create(const evl_wires &wires, const evl_components &comps) 
{
	return create_nets(wires) && create_gates(comps);
}

std::string make_net_name(std::string wire_name, int i) {
	assert(i >= 0);
	std::ostringstream oss;
	oss << wire_name << "[" << i << "]";
	return oss.str();
}//generate "wire_name[i]" as a string

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
//netlist display and save
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

bool netlist::save(std::string &evl_file)
{
	std::string filename = evl_file + ".netlist";
	std::ofstream netlist_file(filename.c_str());
	// verify output_file is ready
	if (!netlist_file)
    {
        std::cerr << "Cannot create " << filename << "!" << std::endl;
        return false;
    }

	netlist_file << nets_.size() << " " << gates_.size() << std::endl;

	for (std::map<std::string, net *>::const_iterator i = nets_.begin();
		i != nets_.end(); ++i)
		(*(i->second)).display(netlist_file);
	
	for (std::list<gate *>::const_iterator j = gates_.begin();
		j != gates_.end(); ++j)
		(**j).display(netlist_file);
	
	return true; 
}

//********************************************************************
//validate semantics
bool flip_flop::validate_structural_semantics() {
	if (pins_.size() != 2) return false;
	//each pin should have a width of 1
	/*
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	*/
	pins_[0]->set_as_output(); 
	pins_[1]->set_as_input();
	return true;
}

bool not_gate::validate_structural_semantics() {
	if (pins_.size() != 2) return false;
	//each pin should have a width of 1
	/*
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	*/
	pins_[0]->set_as_output(); 
	pins_[1]->set_as_input();
	return true;
}

bool buf::validate_structural_semantics() {
	if (pins_.size() != 2) return false;
	//each pin should have a width of 1
	/*
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	*/
	pins_[0]->set_as_output(); 
	pins_[1]->set_as_input();
	return true;
}

bool and_gate::validate_structural_semantics() {
	if (pins_.size() < 3) return false;
	//each pin should have a width of 1
	/*
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	*/
	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i)
		pins_[i]->set_as_input();

	return true;
}

bool or_gate::validate_structural_semantics() {
	if (pins_.size() < 3) return false;
	//each pin should have a width of 1
	/*
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	*/

	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i){
		pins_[i]->set_as_input();
	}
	return true;
}

bool xor_gate::validate_structural_semantics() {
	if (pins_.size() < 3) return false;
	//each pin should have a width of 1
	/*
	for (size_t i = 0; i < pins_.size(); ++i){
		if ((*(pins_[i])).get_pin_index() != 0)
			return false;
	}
	*/

	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i){
		pins_[i]->set_as_input();
	}

	return true;
}

bool one::validate_structural_semantics() {
	if (pins_.size() < 1) return false;
	for (size_t i = 0; i < pins_.size(); ++i)
		pins_[i]->set_as_output();
	return true;
}

bool zero::validate_structural_semantics() {
	if (pins_.size() < 1) return false;
	for (size_t i = 0; i < pins_.size(); ++i)
		pins_[i]->set_as_output();
	return true;
}

bool input::validate_structural_semantics() {
	if (pins_.size() < 1) return false;
	for (size_t i = 0; i < pins_.size(); ++i)
		pins_[i]->set_as_output();
	return true;
}

bool output::validate_structural_semantics() {
	if (pins_.size() < 1) return false;
	for (size_t i = 0; i < pins_.size(); ++i)
		pins_[i]->set_as_input();
	return true;
}

//*****************************************************
bool flip_flop::get_state() {
	return state_;
}//get current state_;

void flip_flop::compute_next_state() {
	net *input_net = pins_[1]->get_net(0);
	next_state_ = input_net->retrieve_logic_value();
}

void flip_flop::on_cycle_begin(){
	//assign next_state_ to state_
	state_ = next_state_;
}

void flip_flop::on_cycle_end(){
	this->compute_next_state();
}

bool input::read_input(std::string &evl_file, int cycles){
	std::string filename = evl_file + "." + this->get_name();
	std::ifstream input_file(filename.c_str());
	if(!input_file){
		std::cerr << "Can't read the input file " <<
		filename << std::endl;
		return false;
	}

	std::string line;
	for (int line_no = 1; std::getline(input_file, line); ++line_no) {
		if(line_no == 1){
			//check if pins and widths match the ones provide by the file
			std::vector<std::string> pin_num = split(line);
			if(pins_.size() != str2num<int>(pin_num[0], std::dec)){
				std::cerr<< "Number of pins doesn't match!" << std::endl;
				return false;
			}
			else{
				for(size_t i = 1; i != pin_num.size(); ++i){
					if(pins_[i-1]->get_nets().size() != 
					str2num<int>(pin_num[i], std::dec)){
						std::cerr << "pin " << i << "width doesn't match!"
						<< std::endl;
						return false;
					}
				}
			}
		}
		else{//from the 2nd line
			std::vector<std::string> pin_values_temp = split(line);
			int num_tran = str2num<int>(pin_values_temp.front(), std::dec);
			//number of transitions
			pin_values.clear();
			for(size_t i = 1; i != pin_values_temp.size(); ++i){
				std::string temp = dec2bin(str2num<int>(pin_values_temp[i], std::hex),
					(pins_[i-1]->get_nets()).size());
				pin_values.push_back(temp);
			}//pin_values contains binary values(string form) for each pin in one cycle

			for(int i = 0; i < num_tran; ++i){
				sim_values.push_back(pin_values);
			}
		}
	}

	std::vector<std::string> last_line = sim_values.back();
	while(sim_values.size() < cycles){
		sim_values.push_back(last_line);
	}//if reaching eof, the last line will be used for the remaining transitions 
		
	return true;
}

void input::on_cycle_begin(){	
	//provide the values from the sim_values
	//assert(!sim_values.empty());
	pin_values = sim_values.front();
	sim_values.erase(sim_values.begin());
}

void output::out_sim_display(std::ofstream &output_file, int cycles)
{
	for(int i = 0; i < cycles; ++i){
		int width = 0;
		for(size_t j = 0; j != pins_.size(); ++j){
			width = pins_[j]->get_nets().size();
			output_file << std::setfill('0') << std::setw(int(ceil(double(width)/4))) 
			<< std::hex << std::uppercase << out_cycles[i][j] << " " ;
		}
		output_file << std::endl;
	}	
}

bool netlist::output_save(std::string &evl_file, int cycles)
{
	for (std::list<gate *>::const_iterator i = gates_.begin();
		i != gates_.end(); ++i){
		if ((*i)->get_type() == "output"){			
			std::string filename = evl_file + "." + (*i)->get_name();
			std::ofstream output_file(filename.c_str());
			// verify output_file is ready
			if (!output_file)
			{
				std::cerr << "Cannot create " << filename << "!" << std::endl;
				return false;
			}
			/* why the code below doesn't work???
			output_file << ((*i)->pins_).size() << std::endl;
			for (std::vector<pin *>::const_iterator pi = ((*i)->pins_).begin();
				pi != ((*i)->pins_).end(); ++pi){
				output_file << "pin " << ((*pi)->get_nets()).size() << " ";
				for (std::vector<net *>::const_iterator ni = ((*pi)->get_nets()).begin();
					ni != ((*pi)->get_nets()).end(); ++ni){
					output_file << (*ni)->get_name() << " ";
				}
				output_file << std::endl;
			}// the common part of each output gate(before the simulation)
			*/

			output_file << (**i).pins_.size() << std::endl;
			for (std::vector<pin *>::const_iterator pi = (**i).pins_.begin();
				pi != (**i).pins_.end(); ++pi){
				output_file << "pin " << (**pi).get_nets().size() << " ";

				for(size_t ni = 0; ni != (**pi).get_nets().size(); ++ni){
					output_file << (**pi).get_nets()[ni]->get_name() << " ";
				}
				
				output_file << std::endl;
			}

			(*i)->out_sim_display(output_file, cycles);
		}

	}
	return true;
}

void output::on_cycle_end(){
	//compute its inputs
	//save the values to a vector
	output_values.clear();
	for(size_t i = 0; i != pins_.size(); ++i){
		int value = 0;
		int width = pins_[i]->get_nets().size();
		for(size_t j = 0; j != width; ++j){
			int bit = 0;
			if(pins_[i]->get_nets()[j]->retrieve_logic_value()){
				bit = 1;
			}
			value += bit * int(pow(2.0,int(j))); 
		}
		output_values.push_back(value); 
	}//contains each pin's value for the current cycle
	
	out_cycles.push_back(output_values);
	//contains pins' values for each cycle
}


//*****************************logic gates***************************
// The Template Method Pattern
bool logic_gate::compute_output() {
	std::vector<bool> inputs;	
	for(size_t i = 1; i != pins_.size(); ++i){ //input pins
		net *input_net = pins_[i]->get_net(0); //the width is only 1 
		//inputs[i-1] = input_net->retrieve_logic_value();
		inputs.push_back(input_net->retrieve_logic_value());
	}// retrieve logic values from all input nets
	bool output = evaluate(inputs);
	return output;
}

bool and_gate::evaluate(const std::vector<bool> &inputs){
	for (size_t i = 0; i <inputs.size(); ++i){
		if(!inputs[i]){
			return false;
		}
	}
	return true;
}

bool or_gate::evaluate(const std::vector<bool> &inputs){
	for (size_t i = 0; i <inputs.size(); ++i){
		if(inputs[i]){
			return true;
		}
	}
	return false;
}

bool xor_gate::evaluate(const std::vector<bool> &inputs){
	int count = 0;
	for (size_t i = 0; i <inputs.size(); ++i){
		if(inputs[i]){
			++count;
		}
	}
	if(count%2){ return true; } //odd number "1" inputs
	else{ return false; }
}

bool not_gate::evaluate(const std::vector<bool> &inputs){
	return !inputs[0]; //only one input
}

bool buf::evaluate(const std::vector<bool> &inputs){
	return inputs[0]; //only one input
}

/*
void one::set_output(){
	for(size_t i = 0; i != this->pins_.size(); ++i){
		for(size_t j = 0; j != (pins_[i]->get_nets()).size(); ++j){
			(pins_[i]->get_nets())[j]->set_logic_value(1);
		}
	}
}

void zero::set_output(){
	for(size_t i = 0; i != this->pins_.size(); ++i){
		for(size_t j = 0; j != (pins_[i]->get_nets()).size(); ++j){
			(pins_[i]->get_nets())[j]->set_logic_value(0);
		}
	}
}
*/

/*
bool and_gate::compute_output() {
	//since we know the structural semantics of gates,
	//i.e. the widths of the input and output pins

	output = true;
	for(size_t i = 1; i != pins_.size(); ++i){ //input pins
		net *input_net = pins_[i]->get_net(0); //the width is only 1 
		if(!(input_net->retrieve_logic_value())){
			output = false;
		}
	}
	return output;
}

bool or_gate::compute_output() {
	output = true;
	for(size_t i = 1; i != pins_.size(); ++i){ //input pins
		net *input_net = pins_[i]->get_net(0); //the width is only 1 
		if(input_net->retrieve_logic_value()){
			output = true;
		}
	}
	return output;
}

bool xor_gate::compute_output() {
	int count = 0; //counting how many input logic values are 1
	for(size_t i = 1; i != pins_.size(); ++i){ //input pins
		net *input_net = pins_[i]->get_net(0); //the width is only 1 
		if(input_net->retrieve_logic_value()){
			++count;
		}
	}
	if(count%2){ output = true; } //odd number "1" inputs
	else{ output = false; }
	return output;
}
*/


//*****************************************************************
bool netlist::simulate(std::string &evl_file, int cycles)
{
	for (int i = 0; i < cycles; ++i)
	{
		//At the begining of each cycle
		//read input files and save values into corresponding containers
		for(std::list<gate *>::const_iterator j = gates_.begin();
			j != gates_.end(); ++j){
			if((*j)->get_type() == "input"){
				if (!(*j)->read_input(evl_file,cycles)){
					return false;
				}
			}
		}
		//set logic values on all nets to '?'(unknown)
		for(std::map<std::string, net *>::const_iterator j = nets_.begin(); 
			j != nets_.end(); ++j){
			j->second->logic_value.first = "?";
		}
		
		for(std::list<gate *>::const_iterator j = gates_.begin();
			j != gates_.end(); ++j){
			(*j)->on_cycle_begin();
		}

		for(std::list<gate *>::const_iterator j = gates_.begin();
			j != gates_.end(); ++j){
			(*j)->on_cycle_end();
		}

	}

	if(output_save(evl_file, cycles)){
		std::cout << "Simulation done. Outputs successfully saved." << std::endl;
		return true;
	}
	else{ 
		std::cout << "Outputs saving failed." << std::endl;
		return false;
	}

}