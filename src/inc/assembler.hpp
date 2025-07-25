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

    CLIContext HandleArgument(char *arg, CLIContext context);
    CLIContext HandleInputFilename(char *arg);
    CLIContext HandleOutputFilename(char *arg);
    CLIContext HandleOutputFileFormat(char *arg);

public:
    bool success;

    Assembler(int argc, char **argv);
};