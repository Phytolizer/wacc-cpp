#include <iostream>
#include <wacc/run.hh>

int main(int argc, char** argv)
{
    return wacc::run({const_cast<const char**>(argv), static_cast<std::size_t>(argc)}, std::cout, std::cerr);
}
