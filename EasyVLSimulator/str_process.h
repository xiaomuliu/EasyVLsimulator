#ifndef GUARD_str_process_h
#define GUARD_str_process_h

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ios>
#include <vector>
#include <string>

template <class T>
T str2num(const std::string& s, 
				std::ios_base& (*f)(std::ios_base&))
{
	T t;
	std::istringstream iss(s);
	iss >> f >> t;
	return t;
}

/*
template <class T>
std::string num2str(T& t, std::ios_base& (*f)(std::ios_base&))
{
	std::stringstream ss;
	ss << f << t;
	return ss.str();
}
*/

std::vector<std::string> split(const std::string&);
std::string dec2bin(unsigned, int);

#endif