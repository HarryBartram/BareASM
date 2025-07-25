#include <iostream>

#include "inc/assembler.hpp"

int main(int argc, char **argv)
{
    Assembler assembler = Assembler(argc, argv);

    if (!assembler.success) {
        std::cerr << "Usage: " << argv[0] << " [FILE] [OPTION?]" << std::endl;
        return 1;
    }

    return 0;
}