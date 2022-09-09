#pragma once

#include <CBaseListener.h>

namespace wacc
{

class ErrorListener : public antlr4::BaseErrorListener
{
  private:
    std::ostream& err;

  public:
    ErrorListener(std::ostream& err);

    void syntaxError(antlr4::Recognizer* recognizer,
        antlr4::Token* offending_symbol,
        size_t line,
        size_t column,
        const std::string& msg,
        std::exception_ptr e) override;
};

} // namespace wacc
