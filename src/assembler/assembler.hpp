#pragma once

enum CommandOptions {
    NONE,
    OUTPUT_FILE,
    FORMAT
};

enum Format {
    BINARY,
    ELF
};

class Assembler {
private:
    std::string outputFile;
    std::string inputFile;
    std::vector<std::string> contents;
    std::map<std::string, std::string> definitions;
    Format format;

    // Command line processing
    static CommandOptions InterpOptionFlag(char *arg);
    void InterpOption(CommandOptions co, char *arg);
    
    // Preprocessing
    static std::vector<std::string> SplitToken(std::string str, std::string delimiter);
    static bool IsInteger(std::string str);
    static std::string ProcessString(std::string str);
    std::string ResolveSymbolsInLine(std::string str);
    bool ProcessIncludeMacro(int line, std::string prefix, std::vector<std::string> tokens);
    bool ProcessDefineMacro(int line, std::vector<std::string> tokens);
    bool ProcessRepeatMacro(int line, std::string prefix, std::vector<std::string> tokens);
    bool ProcessMacro(int line, int character);
    void Preprocess();

public:
    bool success;

    Assembler(int argc, char**argv);

    void Assemble(void);
};