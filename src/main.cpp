#include <driver.hpp>
#include <ast.hpp>

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

void print_expr(const ast::expr_p &ex);

void print_item_path(const ast::item_path &path) {
	for (auto &item: path) {
		std::cout << item.name;
		if (item.templ_args.size()) {
			std::cout << "[";

			for (size_t i = 0; i < item.templ_args.size(); i++) {
				struct {
					void operator()(const ast::expr_p &ex) {
						std::cout << "value ";
						print_expr(ex);
					}

					void operator()(const std::unique_ptr<ast::type_ref> &type) {
						std::cout << "type ";
						print_item_path(type->type_path);
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
			print_expr(e);
			std::cout << "\n";
		}
		depth_--;

		indent_();
		std::cout << "}";
	}

	void operator()(const ast::binary_expr &ex) {
		print_expr(ex.left);
		print_op(ex.type);
		print_expr(ex.right);
	}

	void operator()(const ast::unary_expr &ex) {
		print_op(ex.type);
		print_expr(ex.right);
	}

	void operator()(const ast::if_else_expr &ex) {
		std::cout << "if ";
		print_expr(ex.cond);
		print_expr(ex.then);
		if (ex.else_ex) {
			std::cout << " else ";
			print_expr(ex.else_ex);
		}
	}

	void operator()(const ast::while_expr &ex) {
		std::cout << "while ";
		print_expr(ex.cond);
		print_expr(ex.then);
	}
	
	void operator()(const ast::fn_call_expr &ex) {
		print_expr(ex.fn);
		std::cout << "(";
		for (const auto &e : ex.args)
			print_expr(e);
		std::cout << ")";
	}

	void operator()(const ast::var_decl_expr &ex) {
		std::cout << (ex.is_const ? "const " : "let ");
		std::cout << ex.decl.name << ": ";
		print_item_path(ex.decl.type.type_path);
		std::cout << " = ";
		print_expr(ex.init);
	}

private:
	void indent_() { for (int i = 0; i < depth_; i++) std::cout << "\t"; }

	int depth_ = 0;
} print_expr_impl;

void print_expr(const ast::expr_p &ex) {
	std::cout << "(";
	std::visit(print_expr_impl, ex->ex);
	std::cout << ")";
}

void print_function(const ast::function &fn) {
	std::cout << "fn " << fn.name << "(";

	for (size_t i = 0; i < fn.args.size(); i++) {
		std::cout << fn.args[i].name << ": ";
		print_item_path(fn.args[i].type.type_path);

		if (i < fn.args.size() - 1)
			std::cout << ", ";
	}

	std::cout << "): ";
	print_item_path(fn.return_type.type_path);
	std::cout << " = ";
	print_expr(fn.body);
	std::cout << ";\n\n";
}

int main(int argc, char **argv) {
	if (argc < 2) return 1;

	driver drv;
	if (int r = drv.parse(argv[1]))
		return r;

	for (const auto &fn : drv.functions_)
		print_function(fn);
}
