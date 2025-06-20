#include <iostream>
#include <vector>
#include <map>

#include "assembler/assembler.hpp"

// ASSEMBLER CLASS (HOLD OPTIONS FILENAME ETC), PREPROCESSOR CLASS, TRANSLATOR CLASS, etc
// NEED TO MAINTAIN SYMBOL TABLE.
// USE LOCATION COUNTER AS A TYPE OF PC REGISTER.
// ONCE RUN INTO A LABEL STORE IT IN THE SYMBOL TABLE.

int main(int argc, char **argv)
{
    Assembler assembler = Assembler(argc, argv);
    if (!assembler.success) {
        return -1;
    }

    assembler.Assemble();
    if (!assembler.success) {
        return -2;
    }

    return 0;
}