#pragma once

#include <ostream>
#include <span>

namespace wacc
{

int run(std::span<const char*> args, std::ostream& out, std::ostream& err);

}
