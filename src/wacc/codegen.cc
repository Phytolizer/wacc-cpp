#include "wacc/codegen.hh"

wacc::CodeGenerator::CodeGenerator(std::ostream& output) : output(output)
{
}

void wacc::CodeGenerator::enterProgram(CParser::ProgramContext* ctx)
{
    (void)ctx;
    output << "section .text\n";
    output << "global _start\n";
    output << "_start:\n";
    output << "call main\n";
    output << "mov rdi, rax\n";
    output << "mov rax, 60\n";
    output << "syscall\n";
}

void wacc::CodeGenerator::enterFunction(CParser::FunctionContext* ctx)
{
    output << ctx->ID()->getText() << ":\n";
}

void wacc::CodeGenerator::exitStatement(CParser::StatementContext* ctx)
{
    (void)ctx;
    output << "ret\n";
}

void wacc::CodeGenerator::enterExpression(CParser::ExpressionContext* ctx)
{
    output << "mov rax, " << ctx->getText() << "\n";
}
