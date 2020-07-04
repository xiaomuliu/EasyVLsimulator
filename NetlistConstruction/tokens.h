#ifndef GUARD_tokens_h
#define GUARD_tokens_h

#include <vector>
#include <list>
#include <string>

struct evl_token {
	enum token_type {NAME, NUMBER, SINGLE};
	token_type type;
	std::string str;
	int line_no;
}; // struct evl_token

typedef std::list<evl_token> evl_tokens;

bool extract_tokens_from_file(std::string, evl_tokens&);
//void display_tokens(std::ostream&, const evl_tokens&);
//bool store_tokens_to_file(std::string, const evl_tokens&);
//void count_tokens_by_types(const evl_tokens&);

#endif