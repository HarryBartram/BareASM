#include "../inc/lexer.hpp"

#include <iostream>
#include <algorithm>

// PRIVATE

// Static

bool Lexer::CheckIfInstruction(std::string lexeme)
{
    std::transform(lexeme.begin(), lexeme.end(), lexeme.begin(), ::toupper);
    return lexeme == "ADD" || lexeme == "AND" || lexeme == "CALL" || lexeme == "CLI" || lexeme == "CMP" || lexeme == "DIV" || lexeme == "HLT" || 
    lexeme == "IN" || lexeme == "INC" || lexeme == "INT" || lexeme == "JA" || lexeme == "JAE" || lexeme == "JB" || lexeme == "JBE" || lexeme == "JC" || 
    lexeme == "JCXZ" || lexeme == "JECXZ" || lexeme == "JRCXZ" || lexeme == "JE" || lexeme == "JG" || lexeme == "JGE" || lexeme == "JL" || lexeme == "JLE" || 
    lexeme == "JNA" || lexeme == "JNAE" || lexeme == "JNB" || lexeme == "JNBE" || lexeme == "JNC" || lexeme == "JNE" || lexeme == "JNG" || lexeme == "JNGE" || 
    lexeme == "JNL" || lexeme == "JNLE" || lexeme == "JNO" || lexeme == "JNP" || lexeme == "JNS" || lexeme == "JNZ" || lexeme == "JO" || lexeme == "JP" || 
    lexeme == "JPE" || lexeme == "JPO" || lexeme == "JS"|| lexeme == "JZ" || lexeme == "JMP" || lexeme == "LGDT" || lexeme == "LIDT" || lexeme == "LOOP" || 
    lexeme == "MOV" || lexeme == "MUL" || lexeme == "OR" || lexeme == "OUT" || lexeme == "POP" || lexeme == "PUSH" || lexeme == "RDMSR" || lexeme == "REP" || 
    lexeme == "RET" || lexeme == "SHL" || lexeme == "SHR" || lexeme == "STI" || lexeme == "SYSCALL" || lexeme == "TEST" || lexeme == "WRMSR" || 
    lexeme == "XOR" || lexeme == "DB" || lexeme == "DW" || lexeme == "DD" || lexeme == "DQ"|| lexeme == "TIMES";
}

bool Lexer::CheckIfRegister(std::string lexeme)
{
    std::transform(lexeme.begin(), lexeme.end(), lexeme.begin(), ::toupper);
    return lexeme == "RAX" || lexeme == "EAX" || lexeme == "AX" || lexeme == "AH" || lexeme == "AL" || lexeme == "RBX" || lexeme == "EBX" || lexeme == "BX" || 
    lexeme == "BH" || lexeme == "BL" || lexeme == "RCX" || lexeme == "ECX" || lexeme == "CX" || lexeme == "CH" || lexeme == "CL" || lexeme == "RDX" || 
    lexeme == "EDX" || lexeme == "DX" || lexeme == "DH" || lexeme == "DL" || lexeme == "RSI" || lexeme == "ESI" || lexeme == "SI" || lexeme == "SIL" || 
    lexeme == "RDI" || lexeme == "EDI" || lexeme == "DI" || lexeme == "DIL" || lexeme == "RSP" || lexeme == "ESP" || lexeme == "SP" || lexeme == "SPL" || 
    lexeme == "RBP" || lexeme == "EBP" || lexeme == "BP" || lexeme == "BPL" || lexeme == "R8" || lexeme == "R8D" || lexeme == "R8W" || lexeme == "R8B" || 
    lexeme == "R9" || lexeme == "R9D" || lexeme == "R9W" || lexeme == "R9B" || lexeme == "R10" || lexeme == "R10D" || lexeme == "R10W" || lexeme == "R10B" || 
    lexeme == "R11" || lexeme == "R11D" || lexeme == "R11W" || lexeme == "R11B" || lexeme == "R12" || lexeme == "R12D" || lexeme == "R12W" || lexeme == "R12B" || 
    lexeme == "R13" || lexeme == "R13D" || lexeme == "R13W" || lexeme == "R13B" || lexeme == "R14" || lexeme == "R14D" || lexeme == "R14W" || lexeme == "R14B" || 
    lexeme == "R15" || lexeme == "R15D" || lexeme == "R15W" || lexeme == "R15B" || lexeme == "RIP" || lexeme == "EIP" || lexeme == "IP" || lexeme == "CS" || 
    lexeme == "DS" || lexeme == "SS" || lexeme == "ES" || lexeme == "FS" || lexeme == "GS" || lexeme == "CR0" || lexeme == "CR2" || lexeme == "CR3" || 
    lexeme == "CR4" || lexeme == "CR8";
}

bool Lexer::CheckIfDirective(std::string lexeme)
{
    std::transform(lexeme.begin(), lexeme.end(), lexeme.begin(), ::toupper);
    return lexeme == "SECTION" || lexeme == "BITS" || lexeme == "IMPORT" || lexeme == "EXPORT" || lexeme == "ORG";
}

bool Lexer::CheckIfOperator(std::string lexeme)
{
    return lexeme == "+" || lexeme == "-" || lexeme == "/" || lexeme == "*";
}

bool Lexer::CheckIfValidNumLiteral(std::string lexeme) {
    bool valid = true;

    std::transform(lexeme.begin(), lexeme.end(), lexeme.begin(), ::toupper);
    if (lexeme.substr(0, 2) != "0X") {
        valid = false;
    }

    // Hexadecimal
    if (valid) {
        for (int i = 2; i < lexeme.length(); i++) {
            if (!std::isxdigit(lexeme[i])) {
                return false;
            }
        }
    }

    // Decimal
    if (!valid) {
        for (int i = 0; i < lexeme.length(); i++) {
            if (!std::isdigit(lexeme[i])) {
                return false;
            }
        }
    }

    return true;
}

bool Lexer::CheckIfValidSymbol(std::string lexeme)
{
    // Ensure it does not begin with a number
    if (std::isdigit(lexeme[0])) {
        return false;
    }

    for (int i = 0; i < lexeme.length(); i++) {
        if (!std::isalnum(lexeme[i]) && lexeme[i] != '.' && lexeme[i] != '_' && lexeme[i] != '?' && lexeme[i] != '!' && lexeme[i] != '#') {
            return false;
        }
    }

    return true;
}

// Non-Static

bool Lexer::RegisterLexeme(std::string lexeme)
{
    Token token = {
        lexeme, TokenType::Symbol
    };

    if (lexeme == ":") {
        token.type = TokenType::StatementBreak;
    } else if (lexeme == ",") {
        token.type = TokenType::Comma;
    } else if (CheckIfInstruction(lexeme)) {
        token.type = TokenType::Instruction;
    } else if (CheckIfRegister(lexeme)) {
        token.type = TokenType::Register;
    } else if (CheckIfDirective(lexeme)) {
        token.type = TokenType::Directive;
    } else if (CheckIfOperator(lexeme)) {
        token.type = TokenType::Operator;
    } else if (lexeme == "{") {
        token.type = TokenType::OpenBlock;
    } else if (lexeme == "}") {
        token.type = TokenType::CloseBlock;
    } else if (lexeme == "<") {
        token.type = TokenType::OpenDereference;
    } else if (lexeme == ">") {
        token.type = TokenType::CloseDereference;
    } else if (lexeme == "[") {
        token.type = TokenType::OpenDirective;
    } else if (lexeme == "]") {
        token.type = TokenType::CloseDirective;
    } else if (lexeme == "@") {
        token.type = TokenType::SegmentOverride;
    } else if (lexeme[0] == '"') {
        token.type = TokenType::StringLiteral;
    } else if (CheckIfValidNumLiteral(lexeme)) {
        token.type = TokenType::NumLiteral;
    } else if (!CheckIfValidSymbol(lexeme)) {
        std::cerr << "ERROR: Invalid Symbol: " << lexeme;
        return false;
    }

    this->tokens.push_back(token);
    if (token.type == TokenType::Instruction) {
        std::cout << "INSTRUCTION ADDED: " << token.lexeme << std::endl;
    } else if (token.type == TokenType::Register) {
        std::cout << "REGISTER ADDED: " << token.lexeme << std::endl;
    } else if (token.type == TokenType::Directive) {
        std::cout << "DIRECTIVE ADDED: " << token.lexeme << std::endl;
    } else if (token.type == TokenType::Operator) {
        std::cout << "OPERATOR ADDED: " << token.lexeme << std::endl;
    } else if (token.type == TokenType::Symbol) {
        std::cout << "SYMBOL ADDED: " << token.lexeme << std::endl;
    } else if (token.type == TokenType::NumLiteral) {
        std::cout << "NUMBER ADDED: " << token.lexeme << std::endl;
    } else if (token.type == TokenType::StringLiteral) {
        std::cout << "STRING ADDED: " << token.lexeme << std::endl;
    } else {
        std::cout << "TOKEN ADDED: " << token.lexeme << std::endl;
    }

    return true;
}

bool Lexer::AnalyseLine(std::string line)
{
    std::string lexeme = "";
    bool inQuotes = false;
    bool escaped = false;
    for (int i = 0; i < line.length(); i++) {
        if (!inQuotes) {
            // If character is symbol valid add to lexeme
            if (std::isalnum(line[i]) || line[i] == '_' || line[i] == '!' || line[i] == '?' || line[i] == '.' || line[i] == '#') {
                lexeme += line[i];
            } else if (line[i] == ':' || line[i] == ',' || line[i] == '+' || line[i] == '-' || line[i] == '*' || line[i] == '/' || line[i] == '[' || line[i] == ']' || line[i] == '<' || line[i] == '>' || line[i] == '{' || line[i] == '}' || line[i] == '@') {
                // Only register if lexeme has content
                if (lexeme != "" && !this->RegisterLexeme(lexeme)) {
                    return false;
                }
                lexeme = line[i];
                this->RegisterLexeme(lexeme);
                lexeme = "";
            } else if (line[i] == '"') {
                if (lexeme != "" && !this->RegisterLexeme(lexeme)) {
                    return false;
                }
                lexeme = "";
                lexeme += line[i];
                inQuotes = true;
            } else {
                if (lexeme != "" && !this->RegisterLexeme(lexeme)) {
                    return false;
                }
                lexeme = "";
            }
        } else {
            lexeme += line[i];
            
            if (line[i] == '\\') {
                escaped = !escaped;
            } else if (!escaped && line[i] == '"') {
                inQuotes = false;
                this->RegisterLexeme(lexeme);
                lexeme = "";
            } else {
                escaped = false;
            }
        }
    }

    return true;
}

// PUBLIC

Lexer::Lexer(std::vector<std::string> &data)
{
    this->data = &data;
}

bool Lexer::Analyse()
{
    for (int i = 0; i < this->data->size(); i++) {
        if (!this->AnalyseLine((*this->data)[i])) {
            std::cerr << ", on line: " << i+1 << std::endl;
            return false;
        }
    }

    return true;
}