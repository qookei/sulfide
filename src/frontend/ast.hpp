#pragma once

#include <cstdint>
#include <type_traits>
#include <optional>
#include <cassert>
#include <variant>
#include <vector>
#include <memory>

namespace ast {

struct expr;
using expr_p = std::unique_ptr<expr>;

struct item_ref;

using item_path = std::vector<item_ref>;

enum class type_kind {
	unqualified,
	ref,
	const_ref
};

struct type_ref {
	item_path type_path;
	type_kind kind;
};

using template_arg = std::variant<
	std::unique_ptr<type_ref>,
	std::unique_ptr<expr>
>;

struct item_ref {
	std::string name;
	std::vector<template_arg> templ_args;
};

struct var_decl {
	std::string name;
	type_ref type;
};

enum class op_type {
	null, assign, add, sub, mul, div, deref, lt, gt
};

struct int_expr {
	uint64_t value;
};

struct item_ref_expr {
	item_path path;
};

struct block_expr {
	std::vector<std::unique_ptr<expr>> exprs;
};

struct binary_expr {
	op_type type;
	std::unique_ptr<expr> left;
	std::unique_ptr<expr> right;
};

struct unary_expr {
	op_type type;
	std::unique_ptr<expr> right;
};

struct if_else_expr {
	std::unique_ptr<expr> cond;
	std::unique_ptr<expr> then;
	std::unique_ptr<expr> else_ex = nullptr;
};

struct while_expr {
	std::unique_ptr<expr> cond;
	std::unique_ptr<expr> then;
};

struct fn_call_expr {
	std::unique_ptr<expr> fn;
	std::vector<std::unique_ptr<expr>> args;
};

struct var_decl_expr {
	bool is_const;
	var_decl decl;
	std::unique_ptr<expr> init;
};

struct expr {
	std::variant<
		int_expr,
		item_ref_expr,
		block_expr,
		binary_expr,
		unary_expr,
		if_else_expr,
		while_expr,
		fn_call_expr,
		var_decl_expr
	> ex;

	bool used = false;
};

template <typename T>
auto into_expr(T &&t) {
	return std::make_unique<expr>(std::forward<T>(t));
}

using type_constraints = std::vector<item_path>;

struct template_decl_arg {
	std::string name;
	std::variant<
		type_ref, /* constant value */
		type_constraints /* type */
	> value;
};

using template_decl = std::vector<template_decl_arg>;

struct function {
	std::string name;
	template_decl templ;
	std::vector<var_decl> args;
	type_ref return_type;
	std::unique_ptr<expr> body;
};

} // namespace ast
