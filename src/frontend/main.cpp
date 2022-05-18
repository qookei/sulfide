#include <frontend/parse-driver.hpp>
#include <frontend/debug.hpp>

#include <conflict/conflict.hpp>
#include <flags.hpp>

int main(int argc, char **argv) {
	uint64_t flags;

	const auto arg_parser = conflict::parser{
		conflict::option{{'h', "help", "Show help"}, flags, 1},
		conflict::choice{{'d', "debug", "Debug flags"}, conflict::choice_mode::combine, opts::debug_flags,
			conflict::flag{{"lex", "Lexer tracing"}, opts::debug::lex},
			conflict::flag{{"parse", "Parser tracing"}, opts::debug::parse},
			conflict::flag{{"dump-ast", "Dump AST"}, opts::debug::dump_ast},
		}
	};

	arg_parser.apply_defaults();
	auto st = arg_parser.parse(argc - 1, argv + 1, opts::files);

	conflict::default_report(st);

	if (flags & 1) {
		std::cout << "Usage: [options] <files>\n\n";
		arg_parser.print_help();
		return 1;
	}

	if (opts::files.size() < 1) {
		std::cerr << "No input files\n";
		return 1;
	}

	parse_driver drv;

	drv.trace_scanning = opts::debug_flags & opts::debug::lex;
	drv.trace_parsing = opts::debug_flags & opts::debug::parse;

	if (int r = drv.parse(std::string{opts::files[0]}))
		return r;

	if (opts::debug_flags & opts::debug::dump_ast)
		for (const auto &fn : drv.functions_)
			dump_function(fn);
}
