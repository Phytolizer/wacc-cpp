#include <iostream>
#include <wacc/run.hh>

int main(int argc, char** argv)
{
    return wacc::run({argv, static_cast<std::size_t>(argc)});
}
