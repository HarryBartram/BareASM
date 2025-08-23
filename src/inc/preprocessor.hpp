#pragma once

#include <map>
#include <vector>
#include <string>

class Preprocessor {
private:
    std::map<std::string, std::string> definitions;

    // Helper functions
    static std::vector<std::string> SplitString(std::string str);
    static bool IsInteger(std::string str);
    static std::string ProcessString(std::string str, bool &success);
    static bool VerifySymbolSyntax(std::string symbol);

    // Resolve symbols into their values.
    std::string ResolveSymbolsInLine(std::string line);

    // General process function to divert execution to proper handling function
    bool ProcessMacro(int line, int character);

    // Specific process functions.
    bool ProcessIncludeMacro(int line, std::string prefix, std::vector<std::string> tokens);
    bool ProcessDefineMacro(int line, std::vector<std::string> tokens);
    bool ProcessRepeatMacro(int line, std::string prefix, std::vector<std::string> tokens);

public:
    bool success;
    std::vector<std::string> data;

    Preprocessor(std::string filename);

    // Resolve symbols for each line and process each line for macros.
    bool Preprocess();
};