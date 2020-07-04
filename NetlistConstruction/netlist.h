#ifndef GUARD_netlist_h
#define GUARD_netlist_h

#include "components.h"

class netlist;
class gate;
class net;
class pin;

class net {
	friend class netlist;
	//net objects can only be constructed in its members
	//and the members of netlist
	net(const std::string &name) : net_name_(name){}; 
	net(const net &); // no copy
	net &operator=(const net &); // no assignment

	typedef std::pair<pin *, size_t> pin_info; //pin's name and its width
	std::list<pin_info> connections_;
	std::string net_name_; //newly added member

	~net(){};

public:
	void append_pin(pin *, size_t);
	void display(std::ostream &) const;
	std::string get_name() const;
}; // class net


class pin {
	friend class gate;
	pin(const pin &); // no copy
	pin &operator=(const pin &); // no assignment

	gate *gate_; //gate it belongs to
	size_t pin_index_; //its position on the gate
	std::vector<net *> nets_; //nets connected to it
	
	pin(){}; //ctor used in pin::create
	~pin(){};

	bool create(gate *, size_t, const evl_pin &,
		const std::map<std::string, net *> &);
public:
	//newly added
	size_t get_pin_index() const;
	gate* get_gate() const;  

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
}; // class gate


class netlist {
	
	netlist(const netlist &); // no copy
	netlist &operator=(const netlist &); // no assignment
	
	std::list<gate *> gates_;
	std::map<std::string, net *> nets_;
	 
	bool create_nets(const evl_wires &);
	void create_net(std::string);
	bool create_gates(const evl_components &);
	bool create_gate(const evl_component &);

public:
	netlist(){}; //ctor
	~netlist();
	bool create(const evl_wires &, const evl_components &);
	bool save(std::string &);
}; // class netlist


class flip_flop: public gate {
	//some parts are for project 4
	bool state_, next_state_;
	
	bool validate_structural_semantics();
public:
	flip_flop(std::string name)
		: gate("dff", name), state_(false), next_state_(false) {} //ctor
	//void compute_next_state();
}; // class flip_flop

/*
void flip_flop::compute_next_state() {
	net *input_net = pins_[1]->get_net(0);
	next_state_ = input_net->retrieve_logic_value();
}
*/


class and_gate: public gate {
	bool validate_structural_semantics();
public:
	and_gate(std::string name)
		: gate("and", name){} //ctor
}; // class and_gate

class not_gate: public gate {
	bool validate_structural_semantics();
public:
	not_gate(std::string name)
		: gate("not", name){} //ctor
}; // class not_gate

class buf: public gate {
	bool validate_structural_semantics();
public:
	buf(std::string name)
		: gate("buf", name){} //ctor
}; // class buf

class or_gate: public gate {
	bool validate_structural_semantics();
public:
	or_gate(std::string name)
		: gate("or", name){} //ctor
}; // class or_gate

class xor_gate: public gate {
	bool validate_structural_semantics();
public:
	xor_gate(std::string name)
		: gate("xor", name){} //ctor
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
public:
	input(std::string name)
		: gate("input", name){} //ctor
}; // class input

class output: public gate {
	bool validate_structural_semantics();
public:
	output(std::string name)
		: gate("output", name){} //ctor
}; // class output


#endif