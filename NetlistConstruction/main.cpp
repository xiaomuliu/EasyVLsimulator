#include <iostream>
#include <fstream>
#include <string>
#include "wires.h"
#include "components.h"
#include "parse_evl_file.h"
#include "netlist.h"
#include "syntax.h"

int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
        std::cerr << "You should provide a file name." << std::endl;
        return -1;
    } // validate arguments

	evl_wires wires;
	evl_components comps;

	if (!parse_evl_file(argv[1], wires, comps))
		return -1;
	
	netlist nl;
	if (!nl.create(wires, comps))
		return -1;
		
	std::string nl_file = std::string(argv[1])+".netlist";
	if(!nl.save(nl_file)) // save the netlist
		return -1;

	std::cout << "netlist saved to " << nl_file << std::endl;
	return 0;
}