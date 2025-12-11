#ifndef LEXER_H // conditional compilations(checks whether a macro is not defined)
#define LEXER_H

#include "llvm/ADT/StringRef.h"        // encapsulates a pointer to a C string and its length
#include "llvm/Support/MemoryBuffer.h" // read-only access to a block of memory, filled with the content of a file

class Lexer;

class Token
{
    friend class Lexer; // Lexer can access private and protected members of Token

public:
    enum TokenKind : unsigned short
    {
        eoi,            // end of input
        unknown,        // in case of error at the lexical level
        ident,          // identifier
        number,         // integer literal
        float_literal,  // float literal
        assign,         // =
        minus_assign,   // -=
        plus_assign,    // +=
        star_assign,    // *=
        slash_assign,   // /=
        mod_assign,     // %=
        exp_assign,     // ^=
        eq,             // ==
        neq,            // !=
        gt,             // >
        lt,             // <
        gte,            // >=
        lte,            // <=
        log_and,        // &&
        log_or,         // ||
        increment,      // ++
        decrement,      // --
        comma,          // ,
        semicolon,      // ;
        colon,          // :
        plus,           // +
        minus,          // -
        star,           // *
        slash,          // /
        mod,            // %
        exp,            // ^
        l_paren,        // (
        r_paren,        // )
        l_brace,        // {
        r_brace,        // }
        l_bracket,      // [
        r_bracket,      // ]
        arrow,          // ->
        KW_var,         // var
        KW_int,         // int
        KW_bool,        // bool
        KW_float,       // float
        KW_array,       // array
        KW_true,        // true
        KW_false,       // false
        KW_if,          // if
        KW_else,        // else
        KW_for,         // for
        KW_foreach,     // foreach
        KW_in,          // in
        KW_match,       // match
        KW_print,       // print
        KW_ADD,         // ADD
        KW_SUB,         // SUB
        KW_MUL,         // MUL
        KW_DIV,         // DIV
        KW_MOD,         // MOD
        KW_INC,         // INC
        KW_DEC,         // DEC
        KW_PLE,         // PLE
        KW_MIE,         // MIE
        KW_AND,         // AND
        KW_OR,          // OR
        KW_to_int,      // to_int
        KW_to_float,    // to_float
        KW_to_bool,     // to_bool
        KW_abs,         // abs
        KW_length,      // length
        KW_max,         // max
        KW_index,       // index (changed from Index)
        KW_find         // find
    };

private:
    TokenKind Kind;
    llvm::StringRef Text; // points to the start of the text of the token

public:
    TokenKind getKind() const { return Kind; }
    llvm::StringRef getText() const { return Text; }

    // to test if the token is of a certain kind
    bool is(TokenKind K) const { return Kind == K; }
    bool isOneOf(TokenKind K1, TokenKind K2) const
    {
        return is(K1) || is(K2);
    }
    template <typename... Ts>
    bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks)
        const { return is(K1) || isOneOf(K2, Ks...); }
};

class Lexer
{
    const char *BufferStart; // pointer to the beginning of the input
    const char *BufferPtr;   // pointer to the next unprocessed character

public:
    Lexer(const llvm::StringRef &Buffer)
    {
        BufferStart = Buffer.begin();
        BufferPtr = BufferStart;
    }

    void next(Token &token); // return the next token

private:
    void formToken(Token &Result, const char *TokEnd, Token::TokenKind Kind);
};
#endif