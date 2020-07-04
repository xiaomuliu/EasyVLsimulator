#include <ctype.h>

bool is_a_single(char ch)
{
	return ((ch == '(') || (ch == ')') || (ch == '[') || (ch== ']')
		|| (ch == ':') || (ch == ';') || (ch == ','));
}

bool is_a_name(char ch)
{
	return (isalpha(ch) || (ch == '_') || (ch == '\\') || (ch == '.'));
}

