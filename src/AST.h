#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

// Forward declarations of classes used in the AST
class AST;
class Expr;
class Program;
class Declaration;
class Final;
class BinaryOp;
class UnaryOp;
class Assignment;
class Logic;
class Comparison;
class LogicalExpr;
class IfStmt;
class ForStmt;
class ForeachStmt;
class MatchStmt;
class MatchCase;
class PrintStmt;
class FunctionCall;
class ArrayLiteral;
class ArrayAccess;
class SpecialAssignment;

// Data type enumeration
enum class DataType {
    Int,
    Float,
    Bool,
    Array,
    Unknown
};

// ASTVisitor class defines a visitor pattern to traverse the AST
class ASTVisitor
{
public:
  // Virtual visit functions for each AST node type
  virtual void visit(AST &) {}
  virtual void visit(Expr &) {}
  virtual void visit(Program &) {}
  virtual void visit(Final &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(UnaryOp &) = 0;
  virtual void visit(Assignment &) = 0;
  virtual void visit(Declaration &) = 0;
  virtual void visit(Logic &) {}
  virtual void visit(Comparison &) = 0;
  virtual void visit(LogicalExpr &) = 0;
  virtual void visit(IfStmt &) = 0;
  virtual void visit(ForStmt &) = 0;
  virtual void visit(ForeachStmt &) = 0;
  virtual void visit(MatchStmt &) = 0;
  virtual void visit(MatchCase &) = 0;
  virtual void visit(PrintStmt &) = 0;
  virtual void visit(FunctionCall &) = 0;
  virtual void visit(ArrayLiteral &) = 0;
  virtual void visit(ArrayAccess &) = 0;
  virtual void visit(SpecialAssignment &) = 0;
};

// AST class serves as the base class for all AST nodes
class AST
{
public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor &V) = 0;
};

// Expr class represents an expression in the AST
class Expr : public AST
{
public:
  Expr() {}
};

class Logic : public AST
{
public:
  Logic() {}
};

// Program class represents a group of statements in the AST
class Program : public AST
{
  using dataVector = llvm::SmallVector<AST *>;

private:
  dataVector data;

public:
  Program(llvm::SmallVector<AST *> data) : data(data) {}
  Program() = default;

  llvm::SmallVector<AST *> getdata() { return data; }

  dataVector::const_iterator begin() { return data.begin(); }

  dataVector::const_iterator end() { return data.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Declaration class represents a variable declaration with an initializer in the AST
class Declaration : public Program
{
  using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
  using ValueVector = llvm::SmallVector<Expr *, 8>;
  VarVector Vars;
  ValueVector Values;
  DataType Type;

public:
  Declaration(DataType Type, llvm::SmallVector<llvm::StringRef, 8> Vars, llvm::SmallVector<Expr *, 8> Values)
    : Type(Type), Vars(Vars), Values(Values) {}

  DataType getType() { return Type; }

  VarVector::const_iterator varBegin() { return Vars.begin(); }

  VarVector::const_iterator varEnd() { return Vars.end(); }

  ValueVector::const_iterator valBegin() { return Values.begin(); }

  ValueVector::const_iterator valEnd() { return Values.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Final class represents a final value in the AST (identifier, number, float, bool)
class Final : public Expr
{
public:
  enum ValueKind
  {
    Ident,
    Number,
    Float,
    Bool
  };

private:
  ValueKind Kind;
  llvm::StringRef Val;

public:
  Final(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val) {}

  ValueKind getKind() { return Kind; }

  llvm::StringRef getVal() { return Val; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// BinaryOp class represents a binary operation in the AST
class BinaryOp : public Expr
{
public:
  enum Operator
  {
    Plus,
    Minus,
    Mul,
    Div,
    Mod,
    Exp
  };

private:
  Expr *Left;
  Expr *Right;
  Operator Op;

public:
  BinaryOp(Operator Op, Expr *L, Expr *R) : Op(Op), Left(L), Right(R) {}

  Expr *getLeft() { return Left; }

  Expr *getRight() { return Right; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// UnaryOp class represents a unary operation (++, --)
class UnaryOp : public Expr
{
public:
  enum Operator
  {
    Inc,
    Dec
  };

private:
  Expr *Operand;
  Operator Op;

public:
  UnaryOp(Operator Op, Expr *E) : Op(Op), Operand(E) {}

  Expr *getOperand() { return Operand; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Assignment class represents an assignment expression in the AST
class Assignment : public Program
{
public:
  enum AssignKind
  {
    Assign,
    Minus_assign,
    Plus_assign,
    Star_assign,
    Slash_assign,
    Mod_assign,
    Exp_assign
  };

private:
  Final *Left;
  Expr *Right;
  AssignKind AK;

public:
  Assignment(Final *L, Expr *R, AssignKind AK) : Left(L), Right(R), AK(AK) {}

  Final *getLeft() { return Left; }

  Expr *getRight() { return Right; }

  AssignKind getAssignKind() { return AK; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// SpecialAssignment represents statements like ADD x y z, INC x, etc.
class SpecialAssignment : public Program
{
public:
  enum OpKind
  {
    ADD,  // x = y + z
    SUB,  // x = y - z
    MUL,  // x = y * z
    DIV,  // x = y / z
    MOD,  // x = y % z
    INC,  // x++
    DEC,  // x--
    PLE,  // x += y
    MIE,  // x -= y
    AND,  // x = y && z (bool only)
    OR    // x = y || z (bool only)
  };

private:
  OpKind Op;
  llvm::StringRef Dest;
  llvm::StringRef Arg1;
  llvm::StringRef Arg2; // Empty for INC/DEC

public:
  SpecialAssignment(OpKind Op, llvm::StringRef Dest, llvm::StringRef Arg1 = "", llvm::StringRef Arg2 = "")
    : Op(Op), Dest(Dest), Arg1(Arg1), Arg2(Arg2) {}

  OpKind getOpKind() { return Op; }
  llvm::StringRef getDest() { return Dest; }
  llvm::StringRef getArg1() { return Arg1; }
  llvm::StringRef getArg2() { return Arg2; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Comparison class represents a comparison expression in the AST
class Comparison : public Logic
{
public:
  enum Operator
  {
    Equal,
    Not_equal,
    Greater,
    Less,
    Greater_equal,
    Less_equal
  };

private:
  Expr *Left;
  Expr *Right;
  Operator Op;

public:
  Comparison(Expr *L, Expr *R, Operator Op) : Left(L), Right(R), Op(Op) {}

  Expr *getLeft() { return Left; }

  Expr *getRight() { return Right; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// LogicalExpr class represents a logical expression in the AST
class LogicalExpr : public Logic
{
public:
  enum Operator
  {
    And,
    Or,
  };

private:
  Logic *Left;
  Logic *Right;
  Operator Op;

public:
  LogicalExpr(Logic *L, Logic *R, Operator Op) : Left(L), Right(R), Op(Op) {}

  Logic *getLeft() { return Left; }

  Logic *getRight() { return Right; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// IfStmt class represents if-else statements
class IfStmt : public Program
{
  using StmtVector = llvm::SmallVector<AST *, 8>;
  StmtVector ifStmts;
  StmtVector elseStmts;

private:
  Logic *Cond;

public:
  IfStmt(Logic *Cond, llvm::SmallVector<AST *, 8> ifStmts, llvm::SmallVector<AST *, 8> elseStmts)
    : Cond(Cond), ifStmts(ifStmts), elseStmts(elseStmts) {}

  Logic *getCond() { return Cond; }

  StmtVector::const_iterator begin() { return ifStmts.begin(); }
  StmtVector::const_iterator end() { return ifStmts.end(); }

  StmtVector::const_iterator beginElse() { return elseStmts.begin(); }
  StmtVector::const_iterator endElse() { return elseStmts.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// ForStmt class represents for loops
class ForStmt : public Program
{
  using StmtVector = llvm::SmallVector<AST *, 8>;
  StmtVector body;

private:
  Declaration *Init;
  Logic *Cond;
  AST *Increment;

public:
  ForStmt(Declaration *Init, Logic *Cond, AST *Increment, llvm::SmallVector<AST *, 8> body)
    : Init(Init), Cond(Cond), Increment(Increment), body(body) {}

  Declaration *getInit() { return Init; }
  Logic *getCond() { return Cond; }
  AST *getIncrement() { return Increment; }

  StmtVector::const_iterator begin() { return body.begin(); }
  StmtVector::const_iterator end() { return body.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// ForeachStmt class represents foreach loops
class ForeachStmt : public Program
{
  using StmtVector = llvm::SmallVector<AST *, 8>;
  StmtVector body;

private:
  llvm::StringRef Var;
  llvm::StringRef Array;

public:
  ForeachStmt(llvm::StringRef Var, llvm::StringRef Array, llvm::SmallVector<AST *, 8> body)
    : Var(Var), Array(Array), body(body) {}

  llvm::StringRef getVar() { return Var; }
  llvm::StringRef getArray() { return Array; }

  StmtVector::const_iterator begin() { return body.begin(); }
  StmtVector::const_iterator end() { return body.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// MatchCase represents a single case in match statement
class MatchCase : public AST
{
  using StmtVector = llvm::SmallVector<AST *, 8>;
  StmtVector body;

private:
  Expr *Pattern; // null for default case (_)

public:
  MatchCase(Expr *Pattern, llvm::SmallVector<AST *, 8> body)
    : Pattern(Pattern), body(body) {}

  Expr *getPattern() { return Pattern; }
  bool isDefault() { return Pattern == nullptr; }

  StmtVector::const_iterator begin() { return body.begin(); }
  StmtVector::const_iterator end() { return body.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// MatchStmt represents pattern matching
class MatchStmt : public Program
{
  using CaseVector = llvm::SmallVector<MatchCase *, 8>;
  CaseVector cases;

private:
  Expr *Value;

public:
  MatchStmt(Expr *Value, llvm::SmallVector<MatchCase *, 8> cases)
    : Value(Value), cases(cases) {}

  Expr *getValue() { return Value; }

  CaseVector::const_iterator begin() { return cases.begin(); }
  CaseVector::const_iterator end() { return cases.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// PrintStmt represents print statement
class PrintStmt : public Program
{
private:
  Expr *Value;

public:
  PrintStmt(Expr *Value) : Value(Value) {}

  Expr *getValue() { return Value; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// FunctionCall represents function calls like to_int(x), abs(x), etc.
class FunctionCall : public Expr
{
public:
  enum FunctionKind
  {
    ToInt,
    ToFloat,
    ToBool,
    Abs,
    Length,
    Max,
    Index,
    Find
  };

private:
  FunctionKind Func;
  llvm::SmallVector<Expr *, 4> Args;

public:
  FunctionCall(FunctionKind Func, llvm::SmallVector<Expr *, 4> Args)
    : Func(Func), Args(Args) {}

  FunctionKind getFunction() { return Func; }

  llvm::SmallVector<Expr *, 4>::const_iterator argsBegin() { return Args.begin(); }
  llvm::SmallVector<Expr *, 4>::const_iterator argsEnd() { return Args.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// ArrayLiteral represents array literals like [1, 2, 3]
class ArrayLiteral : public Expr
{
private:
  llvm::SmallVector<Expr *, 8> Elements;

public:
  ArrayLiteral(llvm::SmallVector<Expr *, 8> Elements) : Elements(Elements) {}

  llvm::SmallVector<Expr *, 8>::const_iterator begin() { return Elements.begin(); }
  llvm::SmallVector<Expr *, 8>::const_iterator end() { return Elements.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// ArrayAccess represents array indexing like arr[5]
class ArrayAccess : public Expr
{
private:
  llvm::StringRef ArrayName;
  Expr *Index;

public:
  ArrayAccess(llvm::StringRef ArrayName, Expr *Index)
    : ArrayName(ArrayName), Index(Index) {}

  llvm::StringRef getArrayName() { return ArrayName; }
  Expr *getIndex() { return Index; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

#endif
