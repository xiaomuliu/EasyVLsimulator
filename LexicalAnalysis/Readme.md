## Overview
While it is quite natural for human designers to understand a circuit specified in the
EasyVL language in a textual file, the file on the disk contains no more than a stream
of characters. Therefore, for a computer program to interpret this file as a circuit, it
must be able to *understand* the EasyVL language by identifying the various parts,
e.g. gates and wires, of the circuit from the file. This process is formally known as
compiling. 

## Lexical Analysis
The purpose of lexical analysis is to convert a sequence of characters into a sequence
of tokens. A tokens in a computer language is the counterpart of a word in a human
languages. It is the minimal element in the language that has meanings. For example, let's look at the following EasyVL file **test.evl** which describes a simple circuit
consisting of a flip-flop (dff) and a **not** gate.

// a comment
module top;
wire s0, s1;
dff(s0, s1);
not(s1, s0);
endmodule

In this example, both wire and s0 are examples of tokens since the former starts a list
of wires and the later is the name of a wire. On the other hand, neither 0 nor 1 is a
token since they don't mean anything in the context { they are part of the wire names.
There are three types of tokens defined in the EasyVL language as follows.

**NAME**: tokens of this type should start with a letter, _, ., or \, and any following
character should be among those characters or digits.
**NUMBER**: tokens of this type is a sequence of digits.
**SINGLE**: any character among (, ), [, ], :, ,, and ;.

Note that first, the longest match rule applies here so a token should contain as much
character as possible, e.g. s0 should be interpreted as one NAME token instead of
a NAME token followed by a NUMBER token; second, comments and spaces are
removed during the process since they don't mean anything to the simulator.

## Files

.cpp files:

Main.cpp:
1.	Check input arguments (open .evl file)
2.	Extract tokens from the object file
3.	Display tokens
4.	Store tokens into a file (.tokens)

Extract_from_file.cpp:
	Open .evl file, then extract tokens from each line.

Check.cpp:
	Includes two functions: is_a_single, is_a_name. They are used to judge whether a character is a SINGLE token or it satisfies the requirement of being a element of a NAME token.

Extract_from_line.cpp:
	For each line in the input file, it extracts and categorizes a token by: skipping comments and spaces, checking whether it is a SINGLE, if not, then whether it is a NAME, finally deciding if it is a NUMBER. Otherwise return false.

Display_tokens.cpp
	display each type of token on the ostream.

Store_to_file.cpp
	create an output file to store the result. According to polymorphism, it store the token by using function display_tokens.

Header files:

	tokens.h :  define a new data structure:  evl_token
	Extract_from_file.h, Extract_from_line.h, check.h,  Display_tokens.h, Store_to_file.h are function declarations respectively.

Two test files:

counter.evl,  lfsr10.evl both of which are from golden simulator.
