#pragma once

#include <CBaseListener.h>

namespace wacc
{

class CodeGenerator final : public CBaseListener
{
    std::ostream& output;

  public:
    CodeGenerator(std::ostream& output);

    void enterProgram(CParser::ProgramContext* ctx) override;
    void enterFunction(CParser::FunctionContext* ctx) override;
    void exitStatement(CParser::StatementContext* ctx) override;
    void enterExpression(CParser::ExpressionContext* ctx) override;
};

} // namespace wacc
