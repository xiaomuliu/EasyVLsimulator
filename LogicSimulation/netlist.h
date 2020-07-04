#ifndef GUARD_netlist_h
#define GUARD_netlist_h

//#include <utility>
#include "components.h"

class netlist;
class gate;
class net;
class pin;
class logic_gate;

class net {
	friend class netlist;
	//net objects can only be constructed in its members
	//and the members of netlist
	net(const std::string &name) : net_name_(name){}; 
	net(const net &); // no copy
	net &operator=(const net &); // no assignment

	typedef std::pair<pin *, size_t> pin_info; //pin's name and its width
	std::list<pin_info> connections_;
	std::string net_name_; 
	//status(known'Y' and unknown'?') and its logic value
	std::pair <std::string, bool> logic_value; 

	~net(){};

public:
	void append_pin(pin *, size_t);
	void display(std::ostream &) const;
	std::string get_name() const;
	
	//void set_logic_value(bool);
	bool retrieve_logic_value();
	bool retrieve_from_driver();
}; // class net.


class pin {
	friend class gate;
	pin(const pin &); // no copy
	pin &operator=(const pin &); // no assignment

	gate *gate_; //gate it belongs to
	size_t pin_index_; //its position on the gate
	std::vector<net *> nets_; //nets connected to it
	
	// input/output indicator
	int io_flag;

	pin(){}; //ctor used in pin::create
	~pin(){};

	bool create(gate *, size_t, const evl_pin &,
		const std::map<std::string, net *> &);
public:
	size_t get_pin_index() const;
	gate* get_gate() const;  

	std::vector<net *> get_nets() const;
	net* get_net(const size_t) const;
	int get_io_flag() const;
	void set_as_input();
	void set_as_output();

	bool isinput() const;
	bool isoutput() const;

}; // class pin


class gate {
	friend class netlist;
	gate(const gate &); // no copy
	gate &operator=(const gate &); // no assignment

	std::string type_, name_;

	bool create_pins(const evl_pins &,
		const std::map<std::string, net *> &);
	bool create_pin(const evl_pin &, size_t,
		const std::map<std::string, net *> &);
	virtual bool validate_structural_semantics() = 0;

protected:
	std::vector<pin *> pins_; //its pins

	gate::gate(const std::string &type, const std::string &name)
	: type_(type), name_(name){} //ctor
	~gate();

public:
	std::string get_type() const;
	std::string get_name() const;
	void display(std::ostream &) const;

	virtual void on_cycle_begin(){}	
	virtual void on_cycle_end(){}

	virtual bool get_state(){return false;}
	//virtual void set_output();
	virtual bool compute_output(){return false;}
	virtual void out_sim_display(std::ofstream &, int){}
	virtual bool read_input(std::string &, int){return true;}
	virtual std::vector<std::string> get_pin_values()
	{ std::vector<std::string> empty; return empty; }
}; // class gate


class netlist {
	
	netlist(const netlist &); // no copy
	netlist &operator=(const netlist &); // no assignment
	
	std::list<gate *> gates_;
	std::map<std::string, net *> nets_;

	std::string evl_file;
	 
	bool create_nets(const evl_wires &);
	void create_net(std::string);
	bool create_gates(const evl_components &);
	bool create_gate(const evl_component &);

public:
	netlist(){}; //ctor
	~netlist();
	bool create(const evl_wires &, const evl_components &);
	bool save(std::string &);
	
	bool simulate(std::string &, int);
	bool output_save(std::string &, int);
}; // class netlist


//*************************************
class logic_gate : public gate {
	virtual bool evaluate(const std::vector<bool> &inputs) = 0;
protected:
	logic_gate(const std::string &type, const std::string &name)
		: gate(type, name){};
public:
	bool compute_output();
}; //class logic_gate


class flip_flop: public gate {
	bool state_, next_state_;
	
	bool validate_structural_semantics();
public:
	flip_flop(std::string name)
		: gate("dff", name), state_(false), next_state_(false) {} //ctor
	bool get_state();
	void compute_next_state();
	void on_cycle_begin();
	void on_cycle_end(); 
}; // class flip_flop


class and_gate: public logic_gate {
	bool output; // the output pin has a width of one
	bool validate_structural_semantics();
	bool evaluate(const std::vector<bool> &inputs);
public:
	and_gate(std::string name)
		: logic_gate("and", name){} //ctor

}; // class and_gate

class not_gate: public logic_gate {
	bool output;
	bool validate_structural_semantics();
	bool evaluate(const std::vector<bool> &inputs);
public:
	not_gate(std::string name)
		: logic_gate("not", name){} //ctor
}; // class not_gate

class buf: public logic_gate {
	bool output;
	bool validate_structural_semantics();
	bool evaluate(const std::vector<bool> &inputs);
public:
	buf(std::string name)
		: logic_gate("buf", name){} //ctor
}; // class buf

class or_gate: public logic_gate {
	bool output; // the output pin has a width of one
	bool validate_structural_semantics();
	bool evaluate(const std::vector<bool> &inputs);
public:
	or_gate(std::string name)
		: logic_gate("or", name){} //ctor
}; // class or_gate

class xor_gate: public logic_gate {
	bool output; // the output pin has a width of one
	bool validate_structural_semantics();
	bool evaluate(const std::vector<bool> &inputs);
public:
	xor_gate(std::string name)
		: logic_gate("xor", name){} //ctor
}; // class xor_gate

class one: public gate {
	bool validate_structural_semantics();
public:
	one(std::string name)
		: gate("one", name){} //ctor
}; // class one

class zero: public gate {
	bool validate_structural_semantics();
public:
	zero(std::string name)
		: gate("zero", name){} //ctor
}; // class zero

class input: public gate {
	bool validate_structural_semantics();
	std::vector<std::string> pin_values; //in binary form
	std::vector<std::vector<std::string>> sim_values;
public:
	input(std::string name)
		: gate("input", name){} //ctor
	bool read_input(std::string &, int);
	std::vector<std::string> get_pin_values();
	void on_cycle_begin(); 
}; // class input

class output: public gate {
	bool validate_structural_semantics();
	std::vector<int> output_values; //in decimal form
	std::vector<std::vector<int>> out_cycles; 
	//each element saves a ouput_values container
public:
	output(std::string name)
		: gate("output", name){} //ctor
	void out_sim_display(std::ofstream &, int);
	void on_cycle_end(); 
}; // class output

#endif