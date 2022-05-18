#pragma once

#include <frontend/ast.hpp>

void dump_type(const ast::type_ref &tr);
void dump_expr(const ast::expr_p &ex);
void dump_function(const ast::function &fn);
