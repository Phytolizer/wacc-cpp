#include "wacc/error.hh"

#include <rang.hpp>

wacc::ErrorListener::ErrorListener(std::ostream& err) : err(err)
{
}

void wacc::ErrorListener::syntaxError(antlr4::Recognizer* recognizer,
    antlr4::Token* /*offending_symbol*/,
    size_t line,
    size_t column,
    const std::string& msg,
    std::exception_ptr /*e*/)
{
    std::string source_name = recognizer->getInputStream()->getSourceName();
    err << rang::fg::red;
    if (!source_name.empty())
    {
        err << source_name << ":";
    }
    err << line << ":" << column + 1 << ": error: " << msg << '\n';
    err << rang::fg::reset;

    m_nerrors++;
}

std::size_t wacc::ErrorListener::nerrors() const noexcept
{
    return m_nerrors;
}
