#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Lexer.h"
#include "llvm/Support/raw_ostream.h"

class Parser
{
    Lexer &Lex;
    Token Tok;
    bool HasError;

    void error()
    {
        llvm::errs() << "Unexpected: " << Tok.getText() << "\n";
        HasError = true;
    }

    void advance() { Lex.next(Tok); }

    bool expect(Token::TokenKind Kind)
    {
        if (Tok.getKind() != Kind)
        {
            error();
            return true;
        }
        return false;
    }

    bool consume(Token::TokenKind Kind)
    {
        if (expect(Kind))
            return true;
        advance();
        return false;
    }

    Program *parseProgram();
    AST *parseStatement();
    Declaration *parseDec();
    Assignment *parseAssign();
    SpecialAssignment *parseSpecialAssign();
    IfStmt *parseIf();
    ForStmt *parseFor();
    ForeachStmt *parseForeach();
    MatchStmt *parseMatch();
    PrintStmt *parsePrint();
    Expr *parseExpr();
    Expr *parseTerm();
    Expr *parseFactor();
    Expr *parseFinal();
    Expr *parsePrimary();
    Logic *parseLogic();
    Logic *parseComparison();
    FunctionCall *parseFunctionCall();
    ArrayLiteral *parseArrayLiteral();

public:
    Parser(Lexer &Lex) : Lex(Lex), HasError(false)
    {
        advance();
    }

    bool hasError() { return HasError; }

    Program *parse();
};

#endif
