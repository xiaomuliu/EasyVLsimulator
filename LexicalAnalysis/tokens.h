#ifndef GUARD_tokens_h
#define GUARD_tokens_h

#include <vector>
#include <string>

struct evl_token {
	enum token_type {NAME, NUMBER, SINGLE};
	token_type type;
	std::string str;
	int line_no;
}; // struct evl_token

typedef std::vector<evl_token> evl_tokens;

#endif