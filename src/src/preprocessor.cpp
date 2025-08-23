#include "../inc/preprocessor.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

// PRIVATE

// STATIC

std::vector<std::string> Preprocessor::SplitString(std::string str)
{
    size_t start = 0, end;
    std::string token = "", strStore = str;
    std::vector<std::string> tokens;

    bool inQuotes = false;
    bool escaped = false;
    for (int character = 0; character < str.length(); character++) {
        if (!inQuotes && (std::isalnum(str[character]) || str[character] == '_' || str[character] == '!' || str[character] == '?' || str[character] == '#')) {
            token += str[character];
        } else if (!inQuotes && str[character] == '"') {
            inQuotes = true;
            if (token != "") {
                tokens.push_back(token);
                token = "";
            }
            token += '"';
        } else if (inQuotes && !escaped && str[character] == '"') {
            inQuotes = false;
            token += '"';
            tokens.push_back(token);
            token = "";
        } else if (inQuotes && str[character] == '\\') {
            token += '\\';
            escaped = !escaped;
        } else if (inQuotes) {
            token += str[character];
            escaped = false;
        } else if (!inQuotes && str[character] == ';') {
            break;
        } else {
            if (token != "") {
                tokens.push_back(token);
                token = "";
            }
        }
    }

    if (token != "") {
        tokens.push_back(token);
    }

    return tokens;
}

bool Preprocessor::IsInteger(std::string str)
{
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }

    return true;
}

std::string Preprocessor::ProcessString(std::string str, bool &success)
{
    // If does not end/begin with '"' is not enclosed
    if (str[0] != '"' || str[str.length()-1] != '"') {
        success = false;
        return "";
    }

    // Calculate escape remove quotes, etc
    std::string processedValue = "";
    bool escaped = false;
    for (int i = 1; i < str.length()-1; i++) {
        if (escaped) {
            if (str[i] == '\\') {
                processedValue += '\\';
            } else if (str[i] == '"') {
                processedValue += '"';
            } else if (str[i] == 't') {
                processedValue += '\t';
            }
            escaped = false;
        } else if (str[i] == '\\') {
            escaped = true;
        } else {
            processedValue += str[i];
        }
    }

    // If ends escaped string is not enclosed
    if (escaped) {
        success = false;
        return "";
    }

    return processedValue;
}

bool Preprocessor::VerifySymbolSyntax(std::string symbol)
{
    // Expand later if needed
    return !std::isdigit(symbol[0]);
}

// NON STATIC

std::string Preprocessor::ResolveSymbolsInLine(std::string line)
{
    std::string result = "";
    std::string symbol = "";
    std::string tmp = "";

    bool defining = false;
    bool inQuotes = false;
    bool escaped = false;
    for (int character = 0; character < line.length(); character++) {
        // Symbol valid characters
        if (!inQuotes && (std::isalnum(line[character]) || line[character] == '_' || line[character] == '!' || line[character] == '?' || line[character] == '#' || line[character] == '.')) {
            symbol += line[character];
        } else if (!inQuotes && line[character] == '"') {           // Starting Quotes
            symbol = "";
            result += line[character];
            inQuotes = true;
        } else if (inQuotes) {                                      // All characters inside quotes
            result += line[character];
            if (!escaped && line[character] == '\\') {              // Escape character
                escaped = true;
            } else if (!escaped && line[character] == '"') {        // Ending Quotes
                inQuotes = false;
            } else if (escaped) {                                   // Finished escape code
                escaped = false;
            }
        } else if (!inQuotes && line[character] == ';') {           // No point searching for symbols in comments
            break;
        } else {                                                    // All non-symbol character (outside of quotes)
            if (symbol == "") {
                result += line[character];
                continue;
            }

            // If we are in a defining macro add to result and skip so we can re-define symbols
            if (defining) {
                defining = false;
                result += symbol + line[character];
                symbol = "";
                continue;
            }

            // Check if next symbol is going to be defined
            tmp = symbol;
            std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
            if (tmp == "#DEFINE") {
                defining = true;
            }
            
            // Otherwise search for symbol
            if (VerifySymbolSyntax(symbol) && this->definitions[symbol] != "" && !inQuotes) {
                result += this->definitions[symbol];
            } else {
                result += symbol;
            }
            symbol = "";
            result += line[character];
        }
    }

    // If there was no non-symbol character at the end of the line, check the symbol
    if (symbol != "") {
        if (VerifySymbolSyntax(symbol) && this->definitions[symbol] != "") {
            result += this->definitions[symbol];
        } else {
            result += symbol;
        }
    }

    return result;
}

bool Preprocessor::ProcessMacro(int line, int character)
{
    // Get prefix, split string into tokens and remove the macro line
    std::string prefix = this->data[line].substr(0, character);
    std::vector<std::string> tokens = SplitString(this->data[line]);
    this->data.erase(this->data.begin()+line);

    // Refer to the appropriate processing function for each macro
    std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::toupper);
    if (tokens[0] == "#INCLUDE") {
        return this->ProcessIncludeMacro(line, prefix, tokens);
    } else if (tokens[0] == "#DEFINE") {
        return this->ProcessDefineMacro(line, tokens);
    } else if (tokens[0] == "#REPEAT") {
        return this->ProcessRepeatMacro(line, prefix, tokens);
    } else {
        std::cerr << "ERROR: Unknown macro: " << tokens[0] << ", on line: " << line+1 << std::endl;
        return false;
    }

    return true;
}

bool Preprocessor::ProcessIncludeMacro(int line, std::string prefix, std::vector<std::string> tokens)
{
    // Check for two arguments
    if (tokens.size() != 2) {
        std::cerr << "ERROR: Invalid number of arguments in #include macro, should be 2, on line: " << line+1 << std::endl;
        return false;
    }

    // Process filename
    bool valid = true;
    std::string filename = ProcessString(tokens[1], valid);
    if (!valid) {
        std::cerr << "ERROR: Unknown #include macro argument: " << tokens[1] << ", should be encased in quotes, on line: " << line+1 << std::endl;
        return false;
    }

    // Check if file exists
    std::ifstream file(filename);
    if (!file.good()) {
        std::cerr << "ERROR: Include file: " << filename << ", does not exist or is inaccessible, on line: " << line+1 << std::endl;
    }

    // Insert file contents
    std::string text;
    while (std::getline(file, text)) {
        this->data.insert(this->data.begin()+line, text);
        line++;
    }
    file.close();

    return true;
}

bool Preprocessor::ProcessDefineMacro(int line, std::vector<std::string> tokens)
{
    // Check for three arguments
    if (tokens.size() != 3) {
        std::cerr << "ERROR: Invalid number of arguments in #define macro, should be 3, on line: " << line+1 << std::endl;
        return false;
    }

    // Process string
    bool valid = true;
    std::string value = ProcessString(tokens[2], valid);
    if (!valid) {
        std::cerr << "ERROR: Unknown #define macro argument: " << tokens[2] << ", should be encased in quotes, on line: " << line+1 << std::endl;
        return false;
    }

    if (!VerifySymbolSyntax(tokens[1])) {
        std::cerr << "ERROR: Unknown #define macro argument: " << tokens[1] << ", not a valid symbol, on line: " << line+1 << std::endl;
        return false;
    }

    // Add to definitions
    this->definitions[tokens[1]] = value;

    return true;
}

bool Preprocessor::ProcessRepeatMacro(int line, std::string prefix, std::vector<std::string> tokens)
{
    // Check for three arguments
    if (tokens.size() != 3) {
        std::cerr << "ERROR: Invalid number of arguments in #repeat macro, should be 3, on line: " << line+1 << std::endl;
        return false;
    }

    // Check n is integer then turn into integer.
    if (!IsInteger(tokens[1])) {
        std::cerr << "ERROR: Unknown #repeat macro argument: " << tokens[1] << ", should be an integer, on line: " << line+1 << std::endl;
        return false;
    }

    // Process string
    bool valid = true;
    std::string value = prefix + ProcessString(tokens[2], valid);
    if (!valid) {
        std::cerr << "ERROR: Unknown #repeat macro argument: " << tokens[2] << ", should be encased in quotes, on line: " << line+1 << std::endl;
        return false;
    }

    // Insert code
    for (int i = line; i < std::stoi(tokens[1])+line; i++) {
        this->data.insert(this->data.begin()+i, value);
    }

    return true;
}

// PUBLIC

Preprocessor::Preprocessor(std::string filename) : success(true)
{
    // Check file accessibility
    std::ifstream file(filename);
    if (!file.good()) {
        success = false;
        return;
    }

    // Gather input file contents
    std::string text;
    while (std::getline(file, text)) {
        this->data.push_back(text);
    }
    file.close();
}

bool Preprocessor::Preprocess()
{
    for (int line = 0; line < this->data.size(); line++) {
        if (this->data[line] == "") { continue; }   // Skip if empty line

        if (this->definitions.size() != 0) {        // Resolve symbols if we have previously defined any
            this->data[line] = this->ResolveSymbolsInLine(this->data[line]);
        }

        for (int character = 0; character < this->data[line].length(); character++) {
            if (this->data[line][character] != ' ' && this->data[line][character] != '\t' && this->data[line][character] != '#') {
                break;
            } else if (this->data[line][character] == '#') {
                if (!this->ProcessMacro(line, character)) {
                    return false;
                }
                line--;        // In order to process the replaced line/lines
                break;
            }
        }
    }

    return true;
}