#pragma once

#include <string>
#include <map>
#include <parser.hpp>
#include "ast.hpp"

#define YY_DECL \
	yy::parser::symbol_type yylex(driver &drv)

YY_DECL;

struct driver {
	int parse(const std::string &f) {
		file = f;

		location.initialize(&file);
		scan_begin();

		yy::parser parse{*this};
		parse.set_debug_level(trace_parsing);

		int res = parse();

		scan_end();
		return res;
	}

	std::string file;
	bool trace_parsing = true;

	void scan_begin();
	void scan_end();

	bool trace_scanning = true;

	yy::location location;

	std::vector<ast::function> functions_;
};
