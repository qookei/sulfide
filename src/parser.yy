%skeleton "lalr1.cc"
%require "3.8.2"
%header

%define api.token.raw

%define api.token.constructor
%define api.value.type variant
//%define parse.assert

%code requires {
	#include <string>
	#include <ast.hpp>

	struct driver;
}

%param { driver &drv }

%locations

%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {
	#include <driver.hpp>
}

%define api.token.prefix {TOK_}

%token
	FN		"fn"
	IF		"if"
	ELSE		"else"
	WHILE		"while"
	LET		"let"
	CONST		"const"
	ASSIGN		"="
	PLUS		"+"
	MINUS		"-"
	STAR		"*"
	SLASH		"/"
	LPAREN		"("
	RPAREN		")"
	LBRACE		"{"
	RBRACE		"}"
	LBRACKET	"["
	RBRACKET	"]"
	COMMA		","
	DOT		"."
	SEMICOLON	";"
	COLON		":"
	ARROW		"->"
	PATHSEP		"::"
	TYPE		"type"
	LT		"<"
	GT		">"
	VALUE		"value"

;

%token <std::string> IDENTIFIER "identifier"
%token <std::string> STRING "string"
%token <int> NUMBER "number"

%nterm <std::vector<ast::expr_p>> block_inner args_inner;

%nterm <ast::expr_p> block_expr if_expr while_expr let_expr subexpr expr;

%nterm <ast::template_arg> template_arg;
%nterm <std::vector<ast::template_arg>> template_arg_list;

%nterm <ast::item_ref> name;
%nterm <ast::item_path> path;

%nterm <ast::type_ref> type;

%nterm <ast::var_decl> var_spec;
%nterm <std::vector<ast::var_decl>> args_spec_inner;

%%

%start top_level;

%left "+" "-";
%left "*" "/";
%left "<" ">";
%right "=";
%left ".";

block_inner:
	%empty			{ $$ = std::vector<ast::expr_p>{}; }
|	expr			{ $$ = std::vector<ast::expr_p>{}; $$.push_back(std::move($1)); }
|	block_inner ";" expr	{ $1.push_back(std::move($3)); $$ = std::move($1); }
|	block_inner ";"		{ $$ = std::move($1); }

args_inner:
	%empty			{ $$ = std::vector<ast::expr_p>{}; }
|	expr			{ $$ = std::vector<ast::expr_p>{}; $$.push_back(std::move($1)); }
|	args_inner "," expr	{ $1.push_back(std::move($3)); $$ = std::move($1); }

template_arg:
	expr		{ $$ = ast::template_arg{std::move($1)}; }
|	"type" type	{ $$ = ast::template_arg{std::make_unique<ast::type_ref>(std::move($2))}; }

template_arg_list:
	template_arg				{ $$ = std::vector<ast::template_arg>{}; $$.push_back(std::move($1)); }
|	template_arg_list "," template_arg	{ $1.push_back(std::move($3)); $$ = std::move($1); }

name:
	"identifier"				{ $$ = ast::item_ref{std::move($1)}; }
|	"identifier" "[" template_arg_list  "]"	{ $$ = ast::item_ref{std::move($1), std::move($3)}; }

path:
	name		{ $$ = std::vector<ast::item_ref>{}; $$.push_back(std::move($1)); }
|	path "::" name	{ $1.push_back(std::move($3)); $$ = std::move($1); }

type: path	{ $$ = ast::type_ref{std::move($1)}; }

var_spec: "identifier" ":" type	{ $$ = ast::var_decl{std::move($1), std::move($3)}; }

args_spec_inner:
	%empty				{ $$ = std::vector<ast::var_decl>{}; }
|	var_spec			{ $$ = std::vector<ast::var_decl>{}; $$.push_back(std::move($1)); }
|	args_spec_inner "," var_spec	{ $1.push_back(std::move($3)); $$ = std::move($1); }

block_expr: "{" block_inner "}"	{ $$ = ast::into_expr(ast::block_expr{std::move($2)}); }

if_expr:
	"if" expr block_expr			{ $$ = ast::into_expr(ast::if_else_expr{std::move($2), std::move($3)}); }
|	"if" expr block_expr "else" block_expr	{ $$ = ast::into_expr(ast::if_else_expr{std::move($2), std::move($3), std::move($5)}); }

while_expr: "while" expr block_expr	{ $$ = ast::into_expr(ast::while_expr{std::move($2), std::move($3)}); }

let_expr:
	"let" var_spec "=" expr		{ $$ = ast::into_expr(ast::var_decl_expr{false, std::move($2), std::move($4)}); }
|	"const" var_spec "=" expr	{ $$ = ast::into_expr(ast::var_decl_expr{true, std::move($2), std::move($4)}); }

subexpr:
	"number"			{ $$ = ast::into_expr(ast::int_expr{$1}); }
|	path				{ $$ = ast::into_expr(ast::item_ref_expr{std::move($1)}); }
|	"(" expr ")"			{ $$ = std::move($2); }
|	subexpr "(" args_inner ")"	{ $$ = ast::into_expr(ast::fn_call_expr{std::move($1), std::move($3)}); }

expr:
	subexpr		{ $$ = std::move($1); }
|	expr "+" expr	{ $$ = ast::into_expr(ast::binary_expr{ast::op_type::add, std::move($1), std::move($3)}); }
|	expr "-" expr	{ $$ = ast::into_expr(ast::binary_expr{ast::op_type::sub, std::move($1), std::move($3)}); }
|	expr "*" expr	{ $$ = ast::into_expr(ast::binary_expr{ast::op_type::mul, std::move($1), std::move($3)}); }
|	expr "/" expr	{ $$ = ast::into_expr(ast::binary_expr{ast::op_type::div, std::move($1), std::move($3)}); }
|	expr "=" expr	{ $$ = ast::into_expr(ast::binary_expr{ast::op_type::assign, std::move($1), std::move($3)}); }
|	expr "." expr	{ $$ = ast::into_expr(ast::binary_expr{ast::op_type::deref, std::move($1), std::move($3)}); }
|	expr "<" expr	{ $$ = ast::into_expr(ast::binary_expr{ast::op_type::lt, std::move($1), std::move($3)}); }
|	expr ">" expr	{ $$ = ast::into_expr(ast::binary_expr{ast::op_type::gt, std::move($1), std::move($3)}); }
|	if_expr		{ $$ = std::move($1); }
|	while_expr	{ $$ = std::move($1); }
|	block_expr	{ $$ = std::move($1); }
|	let_expr	{ $$ = std::move($1); }

template_item_decl:
	"identifier" "=" "type"
|	"identifier" "=" "value" type

template_item_decls:
	template_item_decl
|	template_item_decls "," template_item_decl

template_decl:
	%empty
|	"[" template_item_decls "]"

fn_decl:
	"fn" "identifier" template_decl "(" args_spec_inner ")" ":" type "=" expr ";"	{ drv.functions_.push_back(ast::function{$2, std::move($5), std::move($8), std::move($10)}); }

top_level_item:
	fn_decl

top_level_items:
	top_level_item
|	top_level_items top_level_item

top_level:
	top_level_items

%%

void yy::parser::error(const location_type &l, const std::string &m) {
	std::cerr << l << ": " << m << "\n";
}
