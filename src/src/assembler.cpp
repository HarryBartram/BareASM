#include "../inc/assembler.hpp"
#include "../inc/preprocessor.hpp"

#include <cstring>
#include <iostream>

// PRIVATE

CLIContext Assembler::HandleArgument(char *arg, CLIContext context)
{
    switch (context) {
        case CLIContext::InputFilename:
            return this->HandleInputFilename(arg);
        case CLIContext::OutputFilename:
            return this->HandleOutputFilename(arg);
        case CLIContext::OutputFileFormat:
            return this->HandleOutputFileFormat(arg);
        default: // This shouldnt happen but it removes a warning.
            this->success = false;
            return CLIContext::InputFilename;
    }
}

CLIContext Assembler::HandleInputFilename(char *arg)
{
    // Check if 'context setting' arguments are present
    if (strcmp(arg, "-o") == 0) {
        return CLIContext::OutputFilename;
    } else if (strcmp(arg, "-f") == 0) {
        return CLIContext::OutputFileFormat;
    }

    // Otherwise set input filename and return
    if (this->inputFilename != "") {
        this->success = false;
    }

    this->inputFilename = arg;

    return CLIContext::InputFilename;
}

CLIContext Assembler::HandleOutputFilename(char *arg)
{
    this->outputFilename = arg;

    return CLIContext::InputFilename;
}

CLIContext Assembler::HandleOutputFileFormat(char *arg)
{
    if (strcmp(arg, "bin") == 0) {
        this->outputFileFormat = FileFormat::BIN;
    } else if (strcmp(arg, "elf64") == 0) {
        this->outputFileFormat = FileFormat::ELF64;
    } else if (strcmp(arg, "elf32") == 0) {
        this->outputFileFormat = FileFormat::ELF32;
    } else {
        this->success = false;
    }

    return CLIContext::InputFilename;
}

// PUBLIC

Assembler::Assembler(int argc, char **argv) : success(true), inputFilename(""), outputFilename("a.out"), outputFileFormat(FileFormat::ELF64)
{
    // Need at least one argument for input filename
    if (argc < 2) {
        this->success = false;
        return;
    }

    CLIContext context = CLIContext::InputFilename;
    for (int i = 1; i < argc; i++) {
        context = this->HandleArgument(argv[i], context);

        if (!this->success) {
            return;
        }
    }

    // Check if all options were fully processed
    if (context != CLIContext::InputFilename) {
        this->success = false;
    }
}

bool Assembler::Assemble()
{
    Preprocessor preprocessor = Preprocessor(this->inputFilename);
    if (!preprocessor.success) {
        std::cerr << "ERROR: Input file, " << this->inputFilename << ", does not exist or is inaccessible" << std::endl;
        return false;
    }

    if (!preprocessor.Preprocess()) {
        return false;
    }

    return true;
}