#include <iostream>
#include "tokens.h"
#include "Extract_from_file.h"
#include "Display_tokens.h"
#include "Store_to_file.h"

using namespace std;

int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
        cerr << "You should provide a file name." << endl;
        return -1;
    }// verify that argv[1] exists

	// extract tokens
	evl_tokens tokens;
	if (!extract_tokens_from_file(argv[1], tokens)) 
		return -1;

	// display tokens 
	display_tokens(cout, tokens); 

	// store them to file
	string out_name = argv[1];
	out_name += ".tokens";
	store_tokens_to_file(out_name, tokens);
}