#include <frontend/debug.hpp>
#include <iostream>

namespace {

void print_op(ast::op_type type) {
	using enum ast::op_type;

	switch (type) {
		case null: std::cout << "(null)"; break;
		case assign: std::cout << " = "; break;
		case add: std::cout << " + "; break;
		case sub: std::cout << " - "; break;
		case mul: std::cout << " * "; break;
		case div: std::cout << " / "; break;
		case deref: std::cout << "."; break;
		case lt: std::cout << " < "; break;
		case gt: std::cout << " > "; break;
	}
}

void print_item_path(const ast::item_path &path) {
	for (auto &item: path) {
		std::cout << item.name;
		if (item.templ_args.size()) {
			std::cout << "[";

			for (size_t i = 0; i < item.templ_args.size(); i++) {
				struct {
					void operator()(const ast::expr_p &ex) {
						std::cout << "value ";
						dump_expr(ex);
					}

					void operator()(const std::unique_ptr<ast::type_ref> &type) {
						std::cout << "type ";
						dump_type(*type);
					}
				} visitor;

				std::visit(visitor, item.templ_args[i]);

				if (i < item.templ_args.size() - 1)
					std::cout << ", ";
			}

			std::cout << "]";
		}
	}
}

struct {
	void operator()(const ast::int_expr &ex) {
		std::cout << ex.value;
	}

	void operator()(const ast::item_ref_expr &ex) {
		print_item_path(ex.path);
	}

	void operator()(const ast::block_expr &ex) {
		std::cout << "{\n";

		depth_++;
		for (const auto &e : ex.exprs) {
			indent_();
			dump_expr(e);
			std::cout << "\n";
		}
		depth_--;

		indent_();
		std::cout << "}";
	}

	void operator()(const ast::binary_expr &ex) {
		dump_expr(ex.left);
		print_op(ex.type);
		dump_expr(ex.right);
	}

	void operator()(const ast::unary_expr &ex) {
		print_op(ex.type);
		dump_expr(ex.right);
	}

	void operator()(const ast::if_else_expr &ex) {
		std::cout << "if ";
		dump_expr(ex.cond);
		dump_expr(ex.then);
		if (ex.else_ex) {
			std::cout << " else ";
			dump_expr(ex.else_ex);
		}
	}

	void operator()(const ast::while_expr &ex) {
		std::cout << "while ";
		dump_expr(ex.cond);
		dump_expr(ex.then);
	}
	
	void operator()(const ast::fn_call_expr &ex) {
		dump_expr(ex.fn);
		std::cout << "(";
		for (const auto &e : ex.args)
			dump_expr(e);
		std::cout << ")";
	}

	void operator()(const ast::var_decl_expr &ex) {
		std::cout << (ex.is_const ? "const " : "let ");
		std::cout << ex.decl.name << ": ";
		dump_type(ex.decl.type);
		std::cout << " = ";
		dump_expr(ex.init);
	}

private:
	void indent_() { for (int i = 0; i < depth_; i++) std::cout << "\t"; }

	int depth_ = 0;
} dump_expr_impl;

} // namespace anonymous

void dump_expr(const ast::expr_p &ex) {
	std::cout << "(";
	std::visit(dump_expr_impl, ex->ex);
	std::cout << ")";
}

void dump_function(const ast::function &fn) {
	std::cout << "fn " << fn.name << "(";

	for (size_t i = 0; i < fn.args.size(); i++) {
		std::cout << fn.args[i].name << ": ";
		dump_type(fn.args[i].type);

		if (i < fn.args.size() - 1)
			std::cout << ", ";
	}

	std::cout << "): ";
	dump_type(fn.return_type);
	std::cout << " = ";
	dump_expr(fn.body);
	std::cout << ";\n\n";
}

void dump_type(const ast::type_ref &type) {
	using enum ast::type_kind;

	switch (type.kind) {
		case unqualified: break;
		case ref: std::cout << "&"; break;
		case const_ref: std::cout << "&const "; break;
	}

	print_item_path(type.type_path);
}
