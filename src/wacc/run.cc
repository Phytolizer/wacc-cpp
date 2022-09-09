#include "wacc/run.hh"

#include "wacc/codegen.hh"
#include "wacc/error.hh"

#include <CLI/CLI.hpp>
#include <CLexer.h>
#include <CParser.h>
#include <antlr4-runtime.h>
#include <fs_ext/temporary_directory.hpp>
#include <rang.hpp>
#include <subprocess.hpp>

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
    {
        fs_ext::TemporaryDirectory temp_dir{"wacc-"};
        std::filesystem::path asm_path = temp_dir.path() / "a.s";
        {
            std::ofstream output_stream{asm_path};
            CodeGenerator gen{output_stream};
            antlr4::tree::ParseTreeWalker::DEFAULT.walk(&gen, tree);
        }

        std::filesystem::path obj_path = temp_dir.path() / "a.o";
        subprocess::CompletedProcess nasm_process =
            subprocess::run({"nasm", "-f", "elf64", "-o", obj_path.string(), asm_path.string()});
        if (nasm_process.returncode != 0)
        {
            return 1;
        }

        subprocess::CompletedProcess ld_process =
            subprocess::run({"ld", "-o", output_path.string(), obj_path.string()});
        if (ld_process.returncode != 0)
        {
            return 1;
        }
    }

    return 0;
}
