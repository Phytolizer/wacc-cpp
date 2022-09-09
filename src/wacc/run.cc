#include "wacc/run.hh"

#include "wacc/codegen.hh"
#include "wacc/error.hh"

#include <CLI/CLI.hpp>
#include <CLexer.h>
#include <CParser.h>
#include <antlr4-runtime.h>
#include <rang.hpp>

int wacc::run(std::span<const char*> args, std::ostream& out, std::ostream& err)
{
    CLI::App app{"What A C Compiler"};
    CLI::Option* file_opt = app.add_option("FILE", "The file to compile")->required()->check(CLI::ExistingFile);
    CLI::Option* output_opt = app.add_option("-o,--output", "The output file")->default_str("a.out");

    try
    {
        app.parse(args.size(), args.data());
    }
    catch (const CLI::ParseError& e)
    {
        out << (e.get_exit_code() == 0 ? rang::fg::blue : rang::fg::red);
        return app.exit(e, out, err);
    }

    std::filesystem::path input_path = file_opt->as<std::string>();
    std::filesystem::path output_path = output_opt->as<std::string>();

    ErrorListener error_listener{err};

    std::ifstream input{input_path};
    antlr4::ANTLRInputStream input_stream{input};
    input_stream.name = input_path.string();
    CLexer lexer{&input_stream};
    lexer.addErrorListener(&error_listener);
    antlr4::CommonTokenStream tokens{&lexer};
    CParser parser{&tokens};
    parser.removeErrorListeners();
    parser.addErrorListener(&error_listener);
    antlr4::tree::ParseTree* tree = parser.program();
    if (error_listener.nerrors() > 0)
    {
        return 1;
    }
    std::ofstream output_stream{output_path};
    CodeGenerator gen{output_stream};
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&gen, tree);

    return 0;
}
