#pragma once

#include <string>
#include <map>
#include <parser.hpp>
#include <frontend/ast.hpp>

#define YY_DECL \
	yy::parser::symbol_type yylex(parse_driver &drv)

YY_DECL;

struct parse_driver {
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
	bool trace_parsing = false;

	void scan_begin();
	void scan_end();

	bool trace_scanning = false;

	yy::location location;

	std::vector<ast::function> functions_;
};
