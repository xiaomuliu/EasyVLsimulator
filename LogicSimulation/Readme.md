## Logic Simulation

With the netlist constructed, we are now ready to design and implement algorithms in our simulator to simulate the corresponding circuit.Recall that we are interested in simulating flip-flop-based synchronous circuits specified in the EasyVL language. Therefore, we will rely on the following FSM model of the circuits to define the meaning of logic simulation for the circuits specified in EasyVL.
* The state of the FSM is represented by the bits stored in the flip-flops.
* The input and the output of the FSM are represented by the input and the output
bits of the circuit respectively.
* A synchronous circuit has a single clock that drives all the flip-flops. There is
exactly one state transition in the FSM for each clock cycle.
* The combinational part of the circuit, i.e. the part obtained by removing all the flip-flops, defines how the output and the next state is computed from the input
and the current state of the FSM.

Leveraging the FSM model, for a flip-flop-based synchronous circuit specified in EasyVL, by logic simulation we mean to determine the outputs of the FSM for a given number of state transitions with a given initial state and given input at each state transition. We can have the following iterative algorithm for logic simulation: there is one state transition for each iteration where the output and the next state of the FSM are computed from the input and the current state, and the next state will then be used as the current state for the next iteration. To simply our simulator design, we assume that first, the initial state of the FSM is the state where all the flip-flops store a logical value of 0; second, only the first 1000 transitions should be simulated.

The correctness of the simulation can be determined by observing the outputs of the FSM. This is achieved by storing the logical values of the circuit outputs at the end of each state transition to files. Note that not all the circuits specified in EasyVL are synchronous circuits.

## Description

The cycle simulation algorithm may looks like:
	-Given an initial state, i.e. the initial bits stored in the flip flops.
	-For each clock cycle
1.	Retrieve the current state from the flip flops
2.	Retrieve the current input
3.	Compute the next state and the output
4.	Store the next state into the flip flops and the output

According to this algorithm, we should focus on step3 and deal with other programming issues. For example, design new members and member functions in the classes we implemented in project3. Besides, we also have to decide how to read input files and write to output files satisfying their required formats.

With the algorithm described above, we can simply add a function in class netlist. The naïve idea would be as following:

void netlist::simulate(int cycles)
{
	for (int i = 0; i < cycles; ++i)
	{
		set logic values on all nets to ?(unknown)
		for each flip flop, assign next_state_ to state_
		for each input gate, decide the values of its outputs
		for each flip flop, compute next_state_ (recursively)
		for each output gate, compute its inputs (recursively)
		and store the values to file
	}
}

However, we'd like the above function can be completed without modifying the definition of the netlist class or letting netlist to know the class types we used for flip flops, input gates, and output gates. Therefore, we introduced two virtual functions to the gate class: void on_cycle_begin() and void on_cycle_end(). The above function simulate can be implemented as:

void netlist::simulate(int cycles)
{
	for (int i = 0; i < cycles; ++i)
	{
		set logic values on all nets to ?(unknown)
		for each gate g:
		g->on_cycle_begin()
		for each gate g:
		g->on_cycle_end()
	}
}

By default (as implemented in gate), both functions do nothing. We need to implement on_cycle_begin for flip flops and input gates and implement on_cycle_end for flip flops and output gates (in their respective classes)

Now, I will discuss how I implemented these functions.

For flip flops, on_cycle_begin() is quite simple: we just need to assign next state to current state.

For input gate, calling on_cycle_begin(),  we will read the its pins' values for current cycle simulation from a container which contains all the simulation value for all cycles. I'll talk about how to get such a container later.

For flip flops, on_cycle_end() will compute next state which will trigger recursive logic evaluation (will be discussed shortly).

For output gate. on_cycle_end() will compute its inputs which also will trigger recursive logic evaluation. Then it'll store the values to file.

Recursive Logic Evaluation
Now let's focus on recursive logic evaluation
The algorithm is as following:
	-Before the evaluation starts, set the logic value of all nets to ?.
	-For each of the flip-flops/output gates, ask the input nets for their values.
	-If a net is asked for its logic value,
		-If the value is computed (not a ?), then return it.
		-Otherwise (the value is ?), the net should ask its driver gate to
		compute the value. The value should then be stored (replacing
		?) and returned.
	-If a gate is asked to compute a logic value,
		-If it is a flip-flop, then it provide the current state bit.
		-If it is an input gate, then it provide the value from the file.
		-If it is an one/zero gate, then it provide 1/0.
		-Otherwise, it should first ask all of its input nets to provide
		their values and then perform predefined computations.

For net, we can design a pair std::pair <std::string, bool> logic_value to represent its status(known' Y' and unknown'?') and logic value.

The rest tricky part is input gate providing the values and other gates performing predefined computations

For input gate providing values, we first find the pin this net connected to, then read this pins value (from we get in on_cycle_begin). Then we find which bit of this pin the net connected to. Therefore we can get it Boolean value.

For other gates predefined computations, in this project, we modified the gate and its derived class slightly. In virtue of template method pattern, we introduced a abstract class: logic gate and concrete class: and_gate, or_gate,etc. Because their behaviors(compute output) are similar.

Input/output programming issues
For input, the implementation is like below:

-Read in corresponding input file.
-The first line is used for checking whether the each pin and its width matches.
-From the second line,
	-split the line and use a container to contain the values
	-get the number of transitions(the first element of the container)
	-The following values will be saved in another container which contains all the simulation values for all the pins.
	-according to the number of transitions n, we need to save the value mentioned above n times
	-if EOF is reached before the simulation terminates, the values from the last line should be used for all the remaining state transitions.

The trick part is we have to some functions to implement converting from string type to int type, and from decimal values to binary values. All these issues I includes in the header file str_process.h

For output, we add two members in output class. One contains the values of pins for one cycle. The other one contains the former container(i.e. For all cycles). All the things left is display them in the output file in the requiring format which may include using std::setfill, std::setw, std::hex.

