#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <map>

#include "assembler.hpp"

// PRIVATE

CommandOptions Assembler::InterpOptionFlag(char *arg)
{
    if (strcmp(arg, "-o") == 0) {
        return CommandOptions::OUTPUT_FILE;
    } else if (strcmp(arg, "-f") == 0) {
        return CommandOptions::FORMAT;
    }

    return CommandOptions::NONE;
}

void Assembler::InterpOption(CommandOptions co, char *arg)
{
    if (co == CommandOptions::OUTPUT_FILE) {
        this->outputFile = arg;
    } else if (co == CommandOptions::FORMAT) {
        if (strcmp(arg, "bin") == 0) {
            this->format = Format::BINARY;
        } else if (strcmp(arg, "elf") == 0) {
            this->format = Format::ELF;
        }
    }
}

std::vector<std::string> Assembler::SplitToken(std::string str, std::string delimiter)
{
    size_t start = 0, end;
    std::string token, strStore = str;
    std::vector<std::string> tokens;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        token = str.substr(start, end-start);
        start = end + 1;
        tokens.push_back(token);
    }

    if (str.find(delimiter, start) == std::string::npos) {
        token = str.substr(start, str.length());
        tokens.push_back(token);
    }

    return tokens;
}

bool Assembler::IsInteger(std::string str)
{
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }

    return true;
}

std::string Assembler::ProcessString(std::string str)
{
    std::string processedValue = "";
    bool escaped = false;
    for (int i = 1; i < str.length()-1; i++) {
        if (escaped) {
            if (str[i] == '\\') {
                processedValue += '\\';
            } else if (str[i] == '"') {
                processedValue += '"';
            }
            escaped = false;
        } else if (str[i] == '\\') {
            escaped = true;
        } else {
            processedValue += str[i];
        }
    }

    return processedValue;
}

// PROBABLY NEED CHECKING THE PROCESS OF 'IN DEFINE' FEATURE
std::string Assembler::ResolveSymbolsInLine(std::string str)
{
    std::string result = "";
    std::string symbol = "";
    std::string tmp = "";

    bool inDefineMacro = false;
    for (char c : str) {
        if (c == ' ' || c == '\t' || c == '+' || c == '-' || c == '*' || c == '/' || c == '\"') {
            if (symbol == "") {
                result += c;
                continue;
            }

            if (inDefineMacro) {
                inDefineMacro = false;
                result += symbol + c;
                symbol = "";
                continue;
            }

            tmp = symbol;
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
            if (tmp == "#DEFINE") {
                inDefineMacro = true;
            }
            
            if (symbol != "") {
                if (this->definitions[symbol] != "") {
                    result += this->definitions[symbol];
                } else {
                    result += symbol;
                }
                symbol = "";
            }
            result += c;
        } else {
            symbol += c;
        }
    }

    if (symbol != "") {
        if (this->definitions[symbol] != "") {
            result += this->definitions[symbol];
        } else {
            result += symbol;
        }
    }

    return result;
}

bool Assembler::ProcessIncludeMacro(int line, std::string prefix, std::vector<std::string> tokens)
{
    if (tokens[1][0] != '"' || tokens[1][tokens[1].length()-1] != '"' || tokens[1][tokens[1].length()-2] == '\\') {
        std::cerr << "ERROR: Unknown macro argument: " << tokens[1] << ", should be encased in quotes, on line: " << line << std::endl;
        return false;
    }
    // Process filename please
    std::ifstream file(this->inputFile);
    std::string text;
    while (std::getline(file, text)) {
        this->contents.insert(this->contents.begin()+line, text);
        line++;
    }
    file.close();

    return true;
}

bool Assembler::ProcessDefineMacro(int line, std::vector<std::string> tokens)
{
    // CHECK SYMBOL SYNTAX
    if (tokens[2][0] != '"') {
        std::cerr << "ERROR: Unknown macro argument: " << tokens[2] << ", should be encased in quotes, on line: " << line << std::endl;
        return false;
    }

    std::string value = tokens[2];
    for (int i = 3; i < tokens.size(); i++) {
        value += " " + tokens[i];
    }

    if (value[value.length()-1] != '"' or (value[value.length()-2] == '\\' && value[value.length()-3] != '\\')) {
        std::cerr << "ERROR: Unknown macro argument: " << tokens[2] << ", should be encased in quotes, on line: " << line << std::endl;
        return false;
    }

    std::string processedValue = ProcessString(value);
    this->definitions[tokens[1]] = processedValue;

    //std::cout << "Defining symbol: !" << tokens[1] << "!, as: " << processedValue << std::endl;

    return true;
}

bool Assembler::ProcessRepeatMacro(int line, std::string prefix, std::vector<std::string> tokens)
{
    // Check n is integer then turn into integer.
    // Get values between quotes (remember to calculate the escape sequences).
    if (!IsInteger(tokens[1])) {
        std::cerr << "ERROR: Unknown macro argument: " << tokens[1] << ", should be an integer, on line: " << line << std::endl;
        return false;
    }

    if (tokens[2][0] != '"') {
        std::cerr << "ERROR: Unknown macro argument: " << tokens[2] << ", should be encased in quotes, on line: " << line << std::endl;
        return false;
    }

    std::string value = tokens[2];
    for (int i = 3; i < tokens.size(); i++) {
        value += " " + tokens[i];
    }

    if (value[value.length()-1] != '"' or (value[value.length()-2] == '\\' && value[value.length()-3] != '\\')) {
        std::cerr << "ERROR: Unknown macro argument: " << tokens[2] << ", should be encased in quotes, on line: " << line << std::endl;
        return false;
    }

    std::string processedValue = prefix + ProcessString(value);

    for (int i = line; i < std::stoi(tokens[1])+line; i++) {
        this->contents.insert(this->contents.begin()+i, processedValue);
    }

    return true;
}

bool Assembler::ProcessMacro(int line, int character)
{
    std::string prefix = this->contents[line].substr(0, character);
    std::vector<std::string> tokens = SplitToken(this->contents[line].substr(character, this->contents[line].length()), " ");
    this->contents.erase(this->contents.begin()+line);

    // Search all tokens for occurance of ; delete all after (even inside token).
    // Delete all empty tokens
    bool comment = false;
    std::string removedOperators;
    for (int i = 0; i != tokens.size(); i++) {
        if (tokens[i] == "" || comment) {
            tokens.erase(tokens.begin()+i);
            i--;
        }

        for (int a = 0; a < tokens[i].length(); a++) {
            if (tokens[i][a] == ';') {
                if (a == 0) {
                    tokens.erase(tokens.begin()+i);
                    i--;
                } else {
                    tokens[i] = tokens[i].substr(0, a);
                }
                comment = true;
            }
        }
    }

    // Refer to the appropriate processing function for each macro
    std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::toupper);
    if (tokens[0] == "#INCLUDE") {
        return this->ProcessIncludeMacro(line, prefix, tokens);
    } else if (tokens[0] == "#DEFINE") {
        return this->ProcessDefineMacro(line, tokens);
    } else if (tokens[0] == "#REPEAT") {
        return this->ProcessRepeatMacro(line, prefix, tokens);
    } else {
        std::cerr << "ERROR: Unknown macro: " << tokens[0] << ", on line: " << line << std::endl;
        return false;
    }
}

void Assembler::Preprocess(void)
{
    // Search for lines beginning with '#' removing trailing whitespace, subtract a to reprocess line if a macro is found
    // Should actually search all line if a symbol table exists for the define macros
    for (int a = 0; a < this->contents.size(); a++) {
        if (this->contents[a] == "") { continue; }          // Ignore empty lines

        this->contents[a] = this->ResolveSymbolsInLine(this->contents[a]);      // NEED TO CREATE THIS FUNCTION (ENSURE SYMBOL DOES NOT OCCUR IN DEFINE MACRO!!!!)

        for (int b = 0; b < this->contents[a].length(); b++) {
            if (this->contents[a][b] != ' ' && this->contents[a][b] != '\t' && this->contents[a][b] != '#') {
                break;
            } else if (this->contents[a][b] == '#') {
                if (!this->ProcessMacro(a, b)) {
                    this->success = false;
                    return;
                }
                a--;        // In order to process the replaced line/lines
                break;
            }
        }
    }

    for (std::string line : this->contents) {
        std::cout << line << std::endl;
    }
}

// PUBLIC

Assembler::Assembler(int argc, char **argv) : outputFile("a.out"), inputFile(""), format(Format::BINARY), success(false)
{
    if (argc < 2) {
        std::cerr << "Usage: " << std::endl;
        return;
    }

    CommandOptions co = CommandOptions::NONE;
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        // Get input file name if no option set
        if (co == CommandOptions::NONE && this->inputFile == "") {
            this->inputFile = arg;
            continue;
        } else {
            // Get option info then reset
            this->InterpOption(co, arg);
            co = CommandOptions::NONE;
            continue;
        }

        // Set option
        co = Assembler::InterpOptionFlag(arg);
    }

    success = true;
}

void Assembler::Assemble(void)
{
    std::ifstream file(this->inputFile);
    std::string text;
    while (std::getline(file, text)) {
        this->contents.push_back(text);
    }
    file.close();

    this->Preprocess();
}