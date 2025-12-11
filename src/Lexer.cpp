#include "Lexer.h"

// classifying characters
namespace charinfo
{
    // ignore whitespaces
    LLVM_READNONE inline bool isWhitespace(char c)
    {
        return c == ' ' || c == '\t' || c == '\f' || c == '\v' ||
               c == '\r' || c == '\n';
    }

    LLVM_READNONE inline bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    LLVM_READNONE inline bool isLetter(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    LLVM_READNONE inline bool isSpecialSign(char c)
    {
        return c == '/' || c == '%' || c == '-' || c == '+' || c == '^' ||
               c == '*' || c == '<' || c == '>' || c == '=' || c == '!';
    }
}

void Lexer::next(Token &token)
{
    while (*BufferPtr && charinfo::isWhitespace(*BufferPtr))
    {
        ++BufferPtr;
    }

    // Skip comments /* ... */
    while (*BufferPtr == '/' && *(BufferPtr + 1) == '*')
    {
        BufferPtr += 2; // Skip /*
        while (*BufferPtr && !(*BufferPtr == '*' && *(BufferPtr + 1) == '/'))
        {
            ++BufferPtr;
        }
        if (*BufferPtr)
        {
            BufferPtr += 2; // Skip */
        }
        // Skip whitespace after comment
        while (*BufferPtr && charinfo::isWhitespace(*BufferPtr))
        {
            ++BufferPtr;
        }
    }

    // make sure we didn't reach the end of input
    if (!*BufferPtr)
    {
        token.Kind = Token::eoi;
        return;
    }
    // collect characters and check for keywords or ident
    if (charinfo::isLetter(*BufferPtr) || *BufferPtr == '_')
    {
        const char *end = BufferPtr + 1;
        while (charinfo::isLetter(*end) || charinfo::isDigit(*end) || *end == '_')
            ++end;
        llvm::StringRef Name(BufferPtr, end - BufferPtr);
        Token::TokenKind kind;

        // Keywords
        if (Name == "var")
            kind = Token::KW_var;
        else if (Name == "int")
            kind = Token::KW_int;
        else if (Name == "bool")
            kind = Token::KW_bool;
        else if (Name == "float")
            kind = Token::KW_float;
        else if (Name == "array")
            kind = Token::KW_array;
        else if (Name == "true")
            kind = Token::KW_true;
        else if (Name == "false")
            kind = Token::KW_false;
        else if (Name == "if")
            kind = Token::KW_if;
        else if (Name == "else")
            kind = Token::KW_else;
        else if (Name == "for")
            kind = Token::KW_for;
        else if (Name == "foreach")
            kind = Token::KW_foreach;
        else if (Name == "in")
            kind = Token::KW_in;
        else if (Name == "match")
            kind = Token::KW_match;
        else if (Name == "print")
            kind = Token::KW_print;
        else if (Name == "ADD")
            kind = Token::KW_ADD;
        else if (Name == "SUB")
            kind = Token::KW_SUB;
        else if (Name == "MUL")
            kind = Token::KW_MUL;
        else if (Name == "DIV")
            kind = Token::KW_DIV;
        else if (Name == "MOD")
            kind = Token::KW_MOD;
        else if (Name == "INC")
            kind = Token::KW_INC;
        else if (Name == "DEC")
            kind = Token::KW_DEC;
        else if (Name == "PLE")
            kind = Token::KW_PLE;
        else if (Name == "MIE")
            kind = Token::KW_MIE;
        else if (Name == "AND")
            kind = Token::KW_AND;
        else if (Name == "OR")
            kind = Token::KW_OR;
        else if (Name == "to_int")
            kind = Token::KW_to_int;
        else if (Name == "to_float")
            kind = Token::KW_to_float;
        else if (Name == "to_bool")
            kind = Token::KW_to_bool;
        else if (Name == "abs")
            kind = Token::KW_abs;
        else if (Name == "length")
            kind = Token::KW_length;
        else if (Name == "max")
            kind = Token::KW_max;
        else if (Name == "index")
            kind = Token::KW_index;
        else if (Name == "find")
            kind = Token::KW_find;
        else
            kind = Token::ident;
        // generate the token
        formToken(token, end, kind);
        return;
    }
    // check for numbers (int and float)
    else if (charinfo::isDigit(*BufferPtr))
    {
        const char *end = BufferPtr + 1;
        bool isFloat = false;

        while (charinfo::isDigit(*end))
            ++end;

        // Check for decimal point
        if (*end == '.' && charinfo::isDigit(*(end + 1)))
        {
            isFloat = true;
            ++end; // skip the dot
            while (charinfo::isDigit(*end))
                ++end;
        }

        formToken(token, end, isFloat ? Token::float_literal : Token::number);
        return;
    }
    else if (charinfo::isSpecialSign(*BufferPtr))
    {
        const char *end = BufferPtr + 1;
        while (charinfo::isSpecialSign(*end))
            ++end;
        llvm::StringRef Sign(BufferPtr, end - BufferPtr);
        Token::TokenKind kind;
        if (Sign == "=")
            kind = Token::assign;
        else if (Sign == "-=")
            kind = Token::minus_assign;
        else if (Sign == "+=")
            kind = Token::plus_assign;
        else if (Sign == "*=")
            kind = Token::star_assign;
        else if (Sign == "/=")
            kind = Token::slash_assign;
        else if (Sign == "%=")
            kind = Token::mod_assign;
        else if (Sign == "^=")
            kind = Token::exp_assign;
        else if (Sign == "==")
            kind = Token::eq;
        else if (Sign == "!=")
            kind = Token::neq;
        else if (Sign == ">=")
            kind = Token::gte;
        else if (Sign == "<=")
            kind = Token::lte;
        else if (Sign == ">")
            kind = Token::gt;
        else if (Sign == "<")
            kind = Token::lt;
        else if (Sign == "&&")
            kind = Token::log_and;
        else if (Sign == "||")
            kind = Token::log_or;
        else if (Sign == "++")
            kind = Token::increment;
        else if (Sign == "--")
            kind = Token::decrement;
        else if (Sign == "->")
            kind = Token::arrow;
        else if (Sign == "+")
            kind = Token::plus;
        else if (Sign == "-")
            kind = Token::minus;
        else if (Sign == "*")
            kind = Token::star;
        else if (Sign == "/")
            kind = Token::slash;
        else if (Sign == "%")
            kind = Token::mod;
        else if (Sign == "^")
            kind = Token::exp;
        else
            kind = Token::unknown;
        // generate the token
        formToken(token, end, kind);
        return;
    }
    
    else
    {
        switch (*BufferPtr)
        {
#define CASE(ch, tok)                         \
    case ch:                                  \
        formToken(token, BufferPtr + 1, tok); \
        break
            CASE('(', Token::l_paren);
            CASE(')', Token::r_paren);
            CASE('{', Token::l_brace);
            CASE('}', Token::r_brace);
            CASE('[', Token::l_bracket);
            CASE(']', Token::r_bracket);
            CASE(';', Token::semicolon);
            CASE(':', Token::colon);
            CASE(',', Token::comma);
#undef CASE
        default:
            formToken(token, BufferPtr + 1, Token::unknown);
        }
        return;
    }
}

void Lexer::formToken(Token &Tok, const char *TokEnd,
                      Token::TokenKind Kind)
{
    Tok.Kind = Kind;
    Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
    BufferPtr = TokEnd;
}
