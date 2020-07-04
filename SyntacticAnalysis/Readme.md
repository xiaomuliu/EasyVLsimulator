## Tokens and Statements

Once the the EasyVL file is converted to a sequence of tokens, syntactic analysis will
group tokens into statements and extract components and interconnects leveraging the
semantics of the statements.

In the EasyVL language, the sequence of tokens is broken down into a sequence of
statements, where each statement should be among the following four types.
**MODULE**: the sequence starts with a NAME token module and ends with a
SINGLE token ;.
**ENDMODULE**: the sequence contains one NAME token endmodule.
**WIRE**: the sequence starts with a NAME token wire and ends with a SINGLE
token ;.
**COMPONENT**: the sequence starts with a NAME token which is not among
module, wire, or endmodule, and ends with a SINGLE token ;.

Note that except the bonus project where hierarchical designs are support, the EasyVL
file should contain exactly one module. Therefore, the first statement must be MOD-
ULE, the last statement must be ENDMODULE, and any statement in between
must be either WIRE or COMPONENT.

## Syntactic Analysis
While the definitions in the previous section can be used to construct statements from
tokens, we need to further specify the internal structure of the statements, i.e. their
syntax, in order to understand their semantics.

Obviously a ENDMODULE statement has no internal structure. A MODULE statement, on the other hand, has a very straightforward structure: there must be three tokens { module, a NAME token given the type of the module, and ;. However, both the WIRE and COMPONENT statements are much more complicated because the former can define arbitrary number of wires and the latter can have many pins, not to
mention that wires and pins could be buses. Therefore, we need a model to specify these two types of statements. Since there is no recursive structure within the syntax, we can use FSMs, i.e. the same model for synchronous circuits, to model them. This is not quite a coincidence since FSM is one
of the most practical computation models.


## Semantics of Pins
Since the pins are used to represent the wires connected to a component, we must make
sure that the wires they refer to do exist and that the bit ranges, if specified, match
the definition of wires. The requirements are summarized in the following rules. In
addition, since you may need to use a single data type to handle the optional msb/lsb
fields for a pin, the rules also specify a method to update them so that they can be
processed consistently later, no matter they are specified in the EasyVL file or not.

* The name of a pin must be the name of some wire.
* If the wire is not a bus (whose width is 1), then both msb/lsb of the pin must be
-1 as being assigned during the transition from PINS to PIN NAME. Note that this rule implies that the range is not specified.
* If the wire is a bus (whose width is at least 2),
	* If neither msb nor lsb is specified (both are -1), then the msb should be
updated to width􀀀1 and the lsb should be updated to 0, since the pin refers
to the whole bus.
 	* If both msb and lsb are specified, then it must be true that width > msb >=
lsb >= 0.
	* If the msb is specified but the lsb is not (the lsb is -1), then it must be true
that width > msb >= 0. The lsb should be updated to be the same as the
msb, since the pin refers to a single bit within the bus.

## Files
The basic flow chart of main function looks like:
1.	Read in .evl file
2.	Extract tokens from file
3.	Group those tokens into statements
4.	Do syntactic analysis as their supposed form
5.	Store the syntax information into a .syntax file.

There are five header files which define data types and declare functions. The corresponding functions are defined in .cpp files respectively.

Tokens.h:
It defines the evl_token and evl_tokens data structures. The only difference from the one used in project 1 is that evl_tokens is defined as a list container.

Tokens.cpp
It is the same as the one in project 1 except that we did not use functions display_tokens and store_tokens_to_ file.

Statements.h
It defines evl_statement which contains statement type like COMPONENT and its corresponding tokens. It also put evl_statement into a list container.

Statements.cpp
The function group_tokens_into_statements as its name suggests, groups every token into its corresponding statement.  Another function move_tokens_to_statement which is inside group_tokens_into_statement would move tokens into a statement once its type is decided.

Wires.h
It defines evl_wires as a map which has wires names(string type) its keys and width as its values.

Wires.cpp
Function process_wire_statement would take each statement in and save wire information into the evl_wires according to wires' FSM which is described in project assignment.
Function display_wires put wires information on the out stream in the following form:
wire wire_name wire_width

Components.h
evl_component has elements component type, component name, and the pins connected to it.
For pin we also define them as a structure which has pin name, bus_msb, and bus_lsb.
All components were saved in a list container: evl_components


Components.cpp
Function process_wire_statement takes in each statement and save component information into the components list. We also need wires as its input in order to check the correction of pins. The processing would be done based on the components' FSM which is described in project assignment. Other than that we need to handle semantics of pins by checking its name, whether it's a bus and updating its msb and lsb. All these issues were specified in project description.

Function display_components displays components in the following form:
component	component_type 	component_name	 number of pins
pin	pin_name	pin_msb	pin_lsb

Syntax.h
evl_syntax is a structure which has three sub-structures we defined: evl_module, evl_wires, evl_components. The last two ones have been discussed above. The first one evl_module has three elements: module type,  total number of wires, and total number of components.

Syntax.cpp
Function process_ syntax would take in each statement. If the statement type is MODULE, we will save its corresponding name. If the statement type is WIRE, we will call process_wire_statement to process it. Likewise, if its type is COMPONENT, we will call process_component_statement.

Function store_syntax to file would create an output file and call display_syntax to save the syntax in its supposed format.
display_syntax would show syntax in the format referred in project assignment. It would call display_wires and display_component which we have talked about above to implement it.
