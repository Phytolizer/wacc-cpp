#include "wacc/error.hh"

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
    if (!source_name.empty())
    {
        err << source_name << ":";
    }
    err << line << ":" << column + 1 << ": error: " << msg << '\n';

    m_nerrors++;
}

std::size_t wacc::ErrorListener::nerrors() const noexcept
{
    return m_nerrors;
}
