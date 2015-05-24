
#include <iostream>
#include <stdext/stdext.h>
#include <stdext/string.h>
//#include <stdext/stream.h>

namespace se = stdext;

int main(int argc, char* argv[])
{
    {
        std::cout << "result = " << se::string::format("self=<%1>, number=<%2>", std::string(argv[(argc - argc)]), 5930) << std::endl;
    }
}
