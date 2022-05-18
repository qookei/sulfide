/* vim: ft=lex
   */
%{
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <frontend/parse-driver.hpp>
#include "parser.hpp"
%}

%option noyywrap nounput noinput batch debug

%{
	yy::parser::symbol_type make_NUMBER(const std::string &s, const yy::parser::location_type &loc);
%}

id [a-zA-Z@?][a-zA-Z_0-9?]*
int [0-9]+
blank [ \t\r]

%{
#	define YY_USER_ACTION loc.columns(yyleng);
%}

%%

%{
	yy::location &loc = drv.location;
	loc.step();
%}

{blank}+	loc.step();
\n+		loc.lines(yyleng); loc.step();

"fn"		return yy::parser::make_FN(loc);
"if"		return yy::parser::make_IF(loc);
"else"		return yy::parser::make_ELSE(loc);
"while"		return yy::parser::make_WHILE(loc);
"let"		return yy::parser::make_LET(loc);
"const"		return yy::parser::make_CONST(loc);
"="		return yy::parser::make_ASSIGN(loc);
"+"		return yy::parser::make_PLUS(loc);
"-"		return yy::parser::make_MINUS(loc);
"*"		return yy::parser::make_STAR(loc);
"/"		return yy::parser::make_SLASH(loc);
"("		return yy::parser::make_LPAREN(loc);
")"		return yy::parser::make_RPAREN(loc);
"{"		return yy::parser::make_LBRACE(loc);
"}"		return yy::parser::make_RBRACE(loc);
"["		return yy::parser::make_LBRACKET(loc);
"]"		return yy::parser::make_RBRACKET(loc);
","		return yy::parser::make_COMMA(loc);
"."		return yy::parser::make_DOT(loc);
";"		return yy::parser::make_SEMICOLON(loc);
":"		return yy::parser::make_COLON(loc);
"->"		return yy::parser::make_ARROW(loc);
"::"		return yy::parser::make_PATHSEP(loc);
"type"		return yy::parser::make_TYPE(loc);
"<"		return yy::parser::make_LT(loc);
">"		return yy::parser::make_GT(loc);
"value"		return yy::parser::make_VALUE(loc);
"&"		return yy::parser::make_AMP(loc);

{int}		return make_NUMBER(yytext, loc);
{id}		return yy::parser::make_IDENTIFIER(yytext, loc);

"#".*		;

.		{
}

<<EOF>>		return yy::parser::make_YYEOF(loc);

%%

yy::parser::symbol_type make_NUMBER(const std::string &s, const yy::parser::location_type &loc) {
	errno = 0;
	long n = strtol (s.c_str(), NULL, 10);

	return yy::parser::make_NUMBER((int) n, loc);
}

void parse_driver::scan_begin() {
	yy_flex_debug = trace_scanning;
	yyin = fopen(file.c_str(), "r");

	if (!yyin) {
		std::cerr << "cannot open " << file << ": " << strerror(errno) << '\n';
		exit(EXIT_FAILURE);
	}
}

void parse_driver::scan_end() {
	fclose(yyin);
}

