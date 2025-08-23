#pragma once

#include <vector>
#include <string>

enum class TokenType {
    Instruction, Symbol, Register, Comma, Operator, StatementBreak, StringLiteral, NumLiteral, OpenDirective, CloseDirective, Directive, OpenBlock, CloseBlock, 
    OpenDereference, CloseDereference, SegmentOverride
};

struct Token {
    std::string lexeme;
    TokenType type;
};

class Lexer {
private:
    std::vector<std::string> *data;

    // Helper functions for checking what type to assign to each lexeme.
    static bool CheckIfInstruction(std::string lexeme);
    static bool CheckIfRegister(std::string lexeme);
    static bool CheckIfDirective(std::string lexeme);
    static bool CheckIfOperator(std::string lexeme);
    static bool CheckIfValidNumLiteral(std::string lexeme);
    static bool CheckIfValidSymbol(std::string lexeme);

    // Assign type to lexeme to create token.
    bool RegisterLexeme(std::string lexeme);

    // Split line into lexemes and call RegisterLexeme for each.
    bool AnalyseLine(std::string line);

public:
    std::vector<Token> tokens;

    Lexer(std::vector<std::string> &data);

    // Call AnalyseLine for each line.
    bool Analyse();
};