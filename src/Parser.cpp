#include "Parser.h"

Program *Parser::parse()
{
    Program *Res = parseProgram();
    return Res;
}

Program *Parser::parseProgram()
{
    llvm::SmallVector<AST *> data;

    while (!Tok.is(Token::eoi))
    {
        AST *stmt = parseStatement();
        if (stmt)
        {
            data.push_back(stmt);
        }
        else
        {
            // Error occurred, skip to end
            while (!Tok.is(Token::eoi))
                advance();
            return nullptr;
        }
    }
    return new Program(data);
}

AST *Parser::parseStatement()
{
    // Variable declaration: var x int; or int x;
    if (Tok.isOneOf(Token::KW_var, Token::KW_int, Token::KW_bool, Token::KW_float, Token::KW_array))
    {
        return parseDec();
    }
    // Special assignments: ADD, SUB, MUL, DIV, MOD, INC, DEC, PLE, MIE, AND, OR
    else if (Tok.isOneOf(Token::KW_ADD, Token::KW_SUB, Token::KW_MUL, Token::KW_DIV, Token::KW_MOD,
                         Token::KW_INC, Token::KW_DEC, Token::KW_PLE, Token::KW_MIE, Token::KW_AND, Token::KW_OR))
    {
        SpecialAssignment *s = parseSpecialAssign();
        if (s && !consume(Token::semicolon))
            return s;
        return nullptr;
    }
    // If statement
    else if (Tok.is(Token::KW_if))
    {
        return parseIf();
    }
    // For loop
    else if (Tok.is(Token::KW_for))
    {
        return parseFor();
    }
    // Foreach loop
    else if (Tok.is(Token::KW_foreach))
    {
        return parseForeach();
    }
    // Match statement
    else if (Tok.is(Token::KW_match))
    {
        return parseMatch();
    }
    // Print statement
    else if (Tok.is(Token::KW_print))
    {
        PrintStmt *p = parsePrint();
        if (p && !consume(Token::semicolon))
            return p;
        return nullptr;
    }
    // Assignment statement
    else if (Tok.is(Token::ident))
    {
        Assignment *a = parseAssign();
        if (a && !consume(Token::semicolon))
            return a;
        return nullptr;
    }
    else
    {
        error();
        return nullptr;
    }
}

Declaration *Parser::parseDec()
{
    DataType type = DataType::Unknown;

    // Parse: var name type = value; OR type name = value;
    bool hasVar = false;
    if (Tok.is(Token::KW_var))
    {
        hasVar = true;
        advance();
    }

    // If we have 'var', next is identifier, then type
    // If no 'var', next is type, then identifier
    if (hasVar)
    {
        // Syntax: var name type = value;
        if (!expect(Token::ident))
            return nullptr;

        llvm::StringRef varName = Tok.getText();
        advance();

        // Now expect type
        if (Tok.is(Token::KW_int))
            type = DataType::Int;
        else if (Tok.is(Token::KW_bool))
            type = DataType::Bool;
        else if (Tok.is(Token::KW_float))
            type = DataType::Float;
        else if (Tok.is(Token::KW_array))
            type = DataType::Array;
        else
        {
            error();
            return nullptr;
        }
        advance();

        // Check for initialization
        Expr *E = nullptr;
        if (Tok.is(Token::assign))
        {
            advance();
            E = parseExpr();
        }

        if (!consume(Token::semicolon))
            return new Declaration(varName, type, E);
        return nullptr;
    }
    else
    {
        // Syntax: type name = value;
        if (Tok.is(Token::KW_int))
            type = DataType::Int;
        else if (Tok.is(Token::KW_bool))
            type = DataType::Bool;
        else if (Tok.is(Token::KW_float))
            type = DataType::Float;
        else if (Tok.is(Token::KW_array))
            type = DataType::Array;
        else
        {
            error();
            return nullptr;
        }
        advance();

        // Parse variable name
        if (!expect(Token::ident))
            return nullptr;

        llvm::StringRef varName = Tok.getText();
        advance();

        // Parse initialization value
        Expr *E = nullptr;
        if (Tok.is(Token::assign))
        {
            advance();
            E = parseExpr();
        }

        if (!consume(Token::semicolon))
            return new Declaration(varName, type, E);
        return nullptr;
    }
}

Assignment *Parser::parseAssign()
{
    if (!Tok.is(Token::ident))
    {
        error();
        return nullptr;
    }

    Final *F = new Final(Final::Ident, Tok.getText());
    advance();

    Assignment::AssignKind AK;

    if (Tok.is(Token::assign))
        AK = Assignment::Assign;
    else if (Tok.is(Token::plus_assign))
        AK = Assignment::Plus_assign;
    else if (Tok.is(Token::minus_assign))
        AK = Assignment::Minus_assign;
    else if (Tok.is(Token::star_assign))
        AK = Assignment::Star_assign;
    else if (Tok.is(Token::slash_assign))
        AK = Assignment::Slash_assign;
    else if (Tok.is(Token::mod_assign))
        AK = Assignment::Mod_assign;
    else if (Tok.is(Token::exp_assign))
        AK = Assignment::Exp_assign;
    else
    {
        error();
        return nullptr;
    }

    advance();
    Expr *E = parseExpr();
    if (!E)
        return nullptr;

    return new Assignment(F, E, AK);
}

SpecialAssignment *Parser::parseSpecialAssign()
{
    SpecialAssignment::OpKind op;

    if (Tok.is(Token::KW_ADD))
        op = SpecialAssignment::ADD;
    else if (Tok.is(Token::KW_SUB))
        op = SpecialAssignment::SUB;
    else if (Tok.is(Token::KW_MUL))
        op = SpecialAssignment::MUL;
    else if (Tok.is(Token::KW_DIV))
        op = SpecialAssignment::DIV;
    else if (Tok.is(Token::KW_MOD))
        op = SpecialAssignment::MOD;
    else if (Tok.is(Token::KW_INC))
        op = SpecialAssignment::INC;
    else if (Tok.is(Token::KW_DEC))
        op = SpecialAssignment::DEC;
    else if (Tok.is(Token::KW_PLE))
        op = SpecialAssignment::PLE;
    else if (Tok.is(Token::KW_MIE))
        op = SpecialAssignment::MIE;
    else if (Tok.is(Token::KW_AND))
        op = SpecialAssignment::AND;
    else if (Tok.is(Token::KW_OR))
        op = SpecialAssignment::OR;
    else
    {
        error();
        return nullptr;
    }

    advance();

    // Parse destination
    if (!Tok.is(Token::ident))
    {
        error();
        return nullptr;
    }
    llvm::StringRef dest = Tok.getText();
    advance();

    // INC and DEC only take one argument
    if (op == SpecialAssignment::INC || op == SpecialAssignment::DEC)
    {
        return new SpecialAssignment(op, dest);
    }

    // PLE and MIE take two arguments
    if (op == SpecialAssignment::PLE || op == SpecialAssignment::MIE)
    {
        if (!Tok.is(Token::ident))
        {
            error();
            return nullptr;
        }
        llvm::StringRef arg1 = Tok.getText();
        advance();
        return new SpecialAssignment(op, dest, arg1);
    }

    // ADD, SUB, MUL, DIV, MOD, AND, OR take three arguments
    if (!Tok.is(Token::ident))
    {
        error();
        return nullptr;
    }
    llvm::StringRef arg1 = Tok.getText();
    advance();

    if (!Tok.is(Token::ident))
    {
        error();
        return nullptr;
    }
    llvm::StringRef arg2 = Tok.getText();
    advance();

    return new SpecialAssignment(op, dest, arg1, arg2);
}

IfStmt *Parser::parseIf()
{
    if (consume(Token::KW_if))
        return nullptr;

    // Parse condition in parentheses
    if (consume(Token::l_paren))
        return nullptr;

    Logic *cond = parseLogic();
    if (!cond)
        return nullptr;

    if (consume(Token::r_paren))
        return nullptr;

    // Parse if body in braces
    if (consume(Token::l_brace))
        return nullptr;

    llvm::SmallVector<AST *, 8> ifStmts;
    while (!Tok.is(Token::r_brace) && !Tok.is(Token::eoi))
    {
        AST *stmt = parseStatement();
        if (!stmt)
            return nullptr;
        ifStmts.push_back(stmt);
    }

    if (consume(Token::r_brace))
        return nullptr;

    // Parse optional else
    llvm::SmallVector<AST *, 8> elseStmts;
    if (Tok.is(Token::KW_else))
    {
        advance();

        // Check for else if
        if (Tok.is(Token::KW_if))
        {
            IfStmt *elseIf = parseIf();
            if (!elseIf)
                return nullptr;
            elseStmts.push_back(elseIf);
        }
        else
        {
            // Regular else block
            if (consume(Token::l_brace))
                return nullptr;

            while (!Tok.is(Token::r_brace) && !Tok.is(Token::eoi))
            {
                AST *stmt = parseStatement();
                if (!stmt)
                    return nullptr;
                elseStmts.push_back(stmt);
            }

            if (consume(Token::r_brace))
                return nullptr;
        }
    }

    return new IfStmt(cond, ifStmts, elseStmts);
}

ForStmt *Parser::parseFor()
{
    if (consume(Token::KW_for))
        return nullptr;

    if (consume(Token::l_paren))
        return nullptr;

    // Parse initialization
    Declaration *init = parseDec();
    if (!init)
        return nullptr;

    // Note: parseDec already consumed the semicolon

    // Parse condition
    Logic *cond = parseLogic();
    if (!cond)
        return nullptr;

    if (consume(Token::semicolon))
        return nullptr;

    // Parse increment (can be assignment or special assignment or unary)
    AST *increment = nullptr;
    if (Tok.is(Token::ident))
    {
        llvm::StringRef varName = Tok.getText();
        advance();

        if (Tok.is(Token::increment))
        {
            advance();
            Final *f = new Final(Final::Ident, varName);
            increment = new UnaryOp(UnaryOp::Inc, f);
        }
        else if (Tok.is(Token::decrement))
        {
            advance();
            Final *f = new Final(Final::Ident, varName);
            increment = new UnaryOp(UnaryOp::Dec, f);
        }
        else
        {
            // It's a regular assignment, backtrack
            Final *f = new Final(Final::Ident, varName);
            Assignment::AssignKind AK;

            if (Tok.is(Token::assign))
                AK = Assignment::Assign;
            else if (Tok.is(Token::plus_assign))
                AK = Assignment::Plus_assign;
            else if (Tok.is(Token::minus_assign))
                AK = Assignment::Minus_assign;
            else
            {
                error();
                return nullptr;
            }

            advance();
            Expr *E = parseExpr();
            if (!E)
                return nullptr;

            increment = new Assignment(f, E, AK);
        }
    }
    else
    {
        error();
        return nullptr;
    }

    if (consume(Token::r_paren))
        return nullptr;

    // Parse body
    if (consume(Token::l_brace))
        return nullptr;

    llvm::SmallVector<AST *, 8> body;
    while (!Tok.is(Token::r_brace) && !Tok.is(Token::eoi))
    {
        AST *stmt = parseStatement();
        if (!stmt)
            return nullptr;
        body.push_back(stmt);
    }

    if (consume(Token::r_brace))
        return nullptr;

    return new ForStmt(init, cond, increment, body);
}

ForeachStmt *Parser::parseForeach()
{
    if (consume(Token::KW_foreach))
        return nullptr;

    if (consume(Token::l_paren))
        return nullptr;

    // Parse variable name
    if (!Tok.is(Token::ident))
    {
        error();
        return nullptr;
    }
    llvm::StringRef var = Tok.getText();
    advance();

    // Parse 'in' keyword
    if (consume(Token::KW_in))
        return nullptr;

    // Parse array name
    if (!Tok.is(Token::ident))
    {
        error();
        return nullptr;
    }
    llvm::StringRef array = Tok.getText();
    advance();

    if (consume(Token::r_paren))
        return nullptr;

    // Parse body
    if (consume(Token::l_brace))
        return nullptr;

    llvm::SmallVector<AST *, 8> body;
    while (!Tok.is(Token::r_brace) && !Tok.is(Token::eoi))
    {
        AST *stmt = parseStatement();
        if (!stmt)
            return nullptr;
        body.push_back(stmt);
    }

    if (consume(Token::r_brace))
        return nullptr;

    return new ForeachStmt(var, array, body);
}

MatchStmt *Parser::parseMatch()
{
    if (consume(Token::KW_match))
        return nullptr;

    // Parse value to match
    Expr *value = parseExpr();
    if (!value)
        return nullptr;

    if (consume(Token::l_brace))
        return nullptr;

    // Parse cases
    llvm::SmallVector<MatchCase *, 8> cases;
    while (!Tok.is(Token::r_brace) && !Tok.is(Token::eoi))
    {
        Expr *pattern = nullptr;

        // Check for default case (_)
        if (Tok.is(Token::ident) && Tok.getText() == "_")
        {
            advance();
            pattern = nullptr; // Default case
        }
        else
        {
            pattern = parseExpr();
            if (!pattern)
                return nullptr;
        }

        // Parse ->
        if (consume(Token::arrow))
            return nullptr;

        // Parse case body (single statement or expression)
        llvm::SmallVector<AST *, 8> caseBody;
        AST *stmt = parseStatement();
        if (!stmt)
            return nullptr;
        caseBody.push_back(stmt);

        cases.push_back(new MatchCase(pattern, caseBody));

        // Optionally consume comma
        if (Tok.is(Token::comma))
            advance();
    }

    if (consume(Token::r_brace))
        return nullptr;

    return new MatchStmt(value, cases);
}

PrintStmt *Parser::parsePrint()
{
    if (consume(Token::KW_print))
        return nullptr;

    if (consume(Token::l_paren))
        return nullptr;

    Expr *value = parseExpr();
    if (!value)
        return nullptr;

    if (consume(Token::r_paren))
        return nullptr;

    return new PrintStmt(value);
}

Expr *Parser::parseExpr()
{
    Expr *Left = parseTerm();
    if (!Left)
        return nullptr;

    while (Tok.isOneOf(Token::plus, Token::minus))
    {
        BinaryOp::Operator Op;
        if (Tok.is(Token::plus))
            Op = BinaryOp::Plus;
        else
            Op = BinaryOp::Minus;

        advance();
        Expr *Right = parseTerm();
        if (!Right)
            return nullptr;

        Left = new BinaryOp(Op, Left, Right);
    }

    return Left;
}

Expr *Parser::parseTerm()
{
    Expr *Left = parseFactor();
    if (!Left)
        return nullptr;

    while (Tok.isOneOf(Token::star, Token::slash, Token::mod))
    {
        BinaryOp::Operator Op;
        if (Tok.is(Token::star))
            Op = BinaryOp::Mul;
        else if (Tok.is(Token::slash))
            Op = BinaryOp::Div;
        else
            Op = BinaryOp::Mod;

        advance();
        Expr *Right = parseFactor();
        if (!Right)
            return nullptr;

        Left = new BinaryOp(Op, Left, Right);
    }

    return Left;
}

Expr *Parser::parseFactor()
{
    Expr *Left = parsePrimary();
    if (!Left)
        return nullptr;

    while (Tok.is(Token::exp))
    {
        advance();
        Expr *Right = parseFactor(); // Right associative
        if (!Right)
            return nullptr;

        Left = new BinaryOp(BinaryOp::Exp, Left, Right);
    }

    return Left;
}

Expr *Parser::parsePrimary()
{
    // Parenthesized expression
    if (Tok.is(Token::l_paren))
    {
        advance();
        Expr *E = parseExpr();
        if (!E)
            return nullptr;
        if (consume(Token::r_paren))
            return nullptr;
        return E;
    }

    // Array literal
    if (Tok.is(Token::l_bracket))
    {
        return parseArrayLiteral();
    }

    // Function calls
    if (Tok.isOneOf(Token::KW_to_int, Token::KW_to_float, Token::KW_to_bool,
                    Token::KW_abs, Token::KW_length, Token::KW_max,
                    Token::KW_index, Token::KW_find))
    {
        return parseFunctionCall();
    }

    // Identifiers, numbers, floats, booleans
    return parseFinal();
}

Expr *Parser::parseFinal()
{
    Expr *Res = nullptr;

    if (Tok.is(Token::number))
    {
        Res = new Final(Final::Number, Tok.getText());
        advance();
    }
    else if (Tok.is(Token::float_literal))
    {
        Res = new Final(Final::Float, Tok.getText());
        advance();
    }
    else if (Tok.is(Token::KW_true) || Tok.is(Token::KW_false))
    {
        Res = new Final(Final::Bool, Tok.getText());
        advance();
    }
    else if (Tok.is(Token::ident))
    {
        llvm::StringRef name = Tok.getText();
        advance();

        // Check for array access
        if (Tok.is(Token::l_bracket))
        {
            advance();
            Expr *index = parseExpr();
            if (!index)
                return nullptr;
            if (consume(Token::r_bracket))
                return nullptr;
            Res = new ArrayAccess(name, index);
        }
        else
        {
            Res = new Final(Final::Ident, name);
        }
    }
    else
    {
        error();
        return nullptr;
    }

    return Res;
}

Logic *Parser::parseLogic()
{
    Logic *Left = parseComparison();
    if (!Left)
        return nullptr;

    while (Tok.isOneOf(Token::log_and, Token::log_or))
    {
        LogicalExpr::Operator Op;
        if (Tok.is(Token::log_and))
            Op = LogicalExpr::And;
        else
            Op = LogicalExpr::Or;

        advance();
        Logic *Right = parseComparison();
        if (!Right)
            return nullptr;

        Left = new LogicalExpr(Left, Right, Op);
    }

    return Left;
}

Logic *Parser::parseComparison()
{
    // Check for parenthesized logic expression
    if (Tok.is(Token::l_paren))
    {
        advance();
        Logic *L = parseLogic();
        if (!L)
            return nullptr;
        if (consume(Token::r_paren))
            return nullptr;
        return L;
    }

    // Parse comparison
    Expr *Left = parseExpr();
    if (!Left)
        return nullptr;

    Comparison::Operator Op;
    if (Tok.is(Token::eq))
        Op = Comparison::Equal;
    else if (Tok.is(Token::neq))
        Op = Comparison::Not_equal;
    else if (Tok.is(Token::gt))
        Op = Comparison::Greater;
    else if (Tok.is(Token::lt))
        Op = Comparison::Less;
    else if (Tok.is(Token::gte))
        Op = Comparison::Greater_equal;
    else if (Tok.is(Token::lte))
        Op = Comparison::Less_equal;
    else
    {
        error();
        return nullptr;
    }

    advance();
    Expr *Right = parseExpr();
    if (!Right)
        return nullptr;

    return new Comparison(Left, Right, Op);
}

FunctionCall *Parser::parseFunctionCall()
{
    FunctionCall::FunctionKind func;

    if (Tok.is(Token::KW_to_int))
        func = FunctionCall::ToInt;
    else if (Tok.is(Token::KW_to_float))
        func = FunctionCall::ToFloat;
    else if (Tok.is(Token::KW_to_bool))
        func = FunctionCall::ToBool;
    else if (Tok.is(Token::KW_abs))
        func = FunctionCall::Abs;
    else if (Tok.is(Token::KW_length))
        func = FunctionCall::Length;
    else if (Tok.is(Token::KW_max))
        func = FunctionCall::Max;
    else if (Tok.is(Token::KW_index))
        func = FunctionCall::Index;
    else if (Tok.is(Token::KW_find))
        func = FunctionCall::Find;
    else
    {
        error();
        return nullptr;
    }

    advance();

    if (consume(Token::l_paren))
        return nullptr;

    // Parse arguments
    llvm::SmallVector<Expr *, 4> args;
    if (!Tok.is(Token::r_paren))
    {
        Expr *arg = parseExpr();
        if (!arg)
            return nullptr;
        args.push_back(arg);

        while (Tok.is(Token::comma))
        {
            advance();
            arg = parseExpr();
            if (!arg)
                return nullptr;
            args.push_back(arg);
        }
    }

    if (consume(Token::r_paren))
        return nullptr;

    return new FunctionCall(func, args);
}

ArrayLiteral *Parser::parseArrayLiteral()
{
    if (consume(Token::l_bracket))
        return nullptr;

    llvm::SmallVector<Expr *, 8> elements;

    if (!Tok.is(Token::r_bracket))
    {
        Expr *elem = parseExpr();
        if (!elem)
            return nullptr;
        elements.push_back(elem);

        while (Tok.is(Token::comma))
        {
            advance();
            elem = parseExpr();
            if (!elem)
                return nullptr;
            elements.push_back(elem);
        }
    }

    if (consume(Token::r_bracket))
        return nullptr;

    return new ArrayLiteral(elements);
}
