#pragma once

#include <string>

enum class CLIContext {
    InputFilename, OutputFilename, OutputFileFormat
};

enum class FileFormat {
    BIN, ELF32, ELF64
};

class Assembler {
private:
    std::string inputFilename;
    std::string outputFilename;
    FileFormat outputFileFormat;

    // Divert execution based on context.
    CLIContext HandleArgument(char *arg, CLIContext context);

    // Handle specific context
    CLIContext HandleInputFilename(char *arg);  // Also used to handle context-setting arguments (as CLIContext::InputFilename is default context).
    CLIContext HandleOutputFilename(char *arg);
    CLIContext HandleOutputFileFormat(char *arg);

public:
    bool success;

    // Call HandleArgument for each *arg in **argv, saving a context variable.
    Assembler(int argc, char **argv);

    // Run each part of assembly process one after another.
    bool Assemble();
};