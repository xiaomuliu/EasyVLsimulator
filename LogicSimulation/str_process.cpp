#include "str_process.h"

std::vector<std::string> split(const std::string& s)
{
	std::vector<std::string> ret;
	typedef std::vector<std::string>::size_type str_sz;
	str_sz i = 0;

	while (i != s.size())
	{	// ignore leading blanks
		while (i != s.size() && isspace(s[i]))
			++i;

		str_sz j = i;
        while (j != s.size() && !isspace(s[j]))
			j++;

		if (j != i)
		{
			ret.push_back(s.substr(i, j-i));
			i = j;
		}		
	}
	return ret;
}//break a string into words, separated from each other by whitespace

std::string dec2bin(unsigned n, int width)
{//convert decimal number to binary, 
//the number of leading 0's is according to the width 
    const size_t size = sizeof(n) * 8;
    char result[size];

    unsigned index = size;
    do {
        result[--index] = '0' + (n & 1);
    } while (n >>= 1);

	std::string bin_str;
    bin_str = std::string(result + index, result + size);
	std::stringstream ss;

	ss << std::setfill('0') << std::setw(width) << bin_str;
	return ss.str();
}