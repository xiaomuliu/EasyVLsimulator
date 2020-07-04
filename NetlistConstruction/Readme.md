## Netlist Construction
By analyzing the semantics of the statements, we have extracted the descriptions of the wires, the components, and their interconnections in a circuit. The objectives is to start designing a data structure called netlist to represent the circuit for simulation and to implement a program to actually construct the netlist from the descriptions of the circuit.

A netlist is consisting of signal nets (or simply nets) that pass signals around and components that perform computations. The reason to have signal nets instead of wires in a netlist is because the former are much easier to handle in simulation when buses are presented. In the netlist, each wire of width k is broken into k signal nets where each net passes exactly one bit of signal. For example, a wire representing a 8-bit bus converts to 8 signal nets in the netlist. 

In order to perform circuit simulation, the simulator relies on the netlist data structure to provide the following functionality. First, one should be able to access all the gates and nets in the circuit. Second, for each gate, one should be able to obtain input signals and to compute output signals. Third, for each net, one should be able to determine the signal from the gates driving it. Here we focuse on the structural representation, i.e. components and interconnections, of the circuit as a netlist. Therefore, the netlist data structure should allow one to access:
* Gates and nets in the circuit;
* Nets connected to each gate;
* Gates connected to each net.

## Files

We follow the principle of objected-oriented design. For the netlist, this method leads to three obvious types of objects: the netlist itself, the gates, and the nets.
We also need one more object--pins to fulfill our design.

Now, here is how we implemented the netlist construction:

Everything we completed previously now is hidden
inside the function parse_evl_file. (Basically it groups all the functions in project2 except for the file-saving part.) Its tasks include:
      1. 	Break the file into tokens
      2. 	Convert tokens to statements
3.	Analyze statements to generate wires and components
4.	Validate and update semantics of pins

After obtaining the wires and components information from parse_evl_file, now we can build our classes. The responsibility of each class is as follows:
	netlist: access gates and nets
	gate: access its pins
	net: access pins it is connected to and its position within each pin
	pin: access the gate it belongs to and its position within the
                  gate, access the nets connected to it

Here we ignore the details of each class (which can be referred in the program files). Basically they all have their constructors and destructors if necessary, their own private members and corresponding accessor functions and some creation functions which would be used in netlist construction.

A netlist object can be constructed as follows:
	1. Create net objects from wires
	2. Create gate objects from components
	3. Create pin objects within each gate
	4. Make connections between pins and nets
	5. Validate the structural semantics of gates

We briefly introduce the functions here:

netlist::create:
Divide the construction work into two sub-functions create_nets and create_gates.

netlist::create_nets:
For each wire element in wires(a map container), call sub-function create_net to create a net

netlist::create_net:
create a new net in nets_(a private map container member) based on its net_name

netlist::create_gates:
For each component in comps(a list container), call sub-function create_gate to create a gate.

netlist::create_gate:
For each input component, according to its type, call corresponding constructor to construct a new gate, then add it to gates_(a private list container member). Finally, call create_pins to create pins' information of this gate.

gate::create_pins:
For each pin in pins(a list container), find its position on the gate and call create_pin. After all pins are done, validate the semantics of this gate based on its rules.

gate::create_pin:
Create a new pin, add it to pins_(a vector member of gate), and then call pin::create to finish all the information about this pin.

pin::create:
Store corresponding information: gate it belongs to, its position on this gate. After that, according to whether it's a 1-bit wire or a bus, make connections between pins and nets.
