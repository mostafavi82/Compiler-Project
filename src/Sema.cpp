#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace nms {
class InputCheck : public ASTVisitor {
  llvm::StringSet<> Scope;
  bool HasError;

  enum ErrorType { Twice, Not };

  void error(ErrorType ET, llvm::StringRef V) {
    llvm::errs() << "Variable " << V << " is "
                 << (ET == Twice ? "already" : "not")
                 << " declared\n";
    HasError = true;
  }

public:
  InputCheck() : HasError(false) {}

  bool hasError() { return HasError; }

  virtual void visit(Program &Node) override {
    for (llvm::SmallVector<AST *>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I)
    {
      (*I)->accept(*this);
    }
  };

  virtual void visit(Final &Node) override {
    if (Node.getKind() == Final::Ident) {
      if (Scope.find(Node.getVal()) == Scope.end())
        error(Not, Node.getVal());
    }
  };

  virtual void visit(BinaryOp &Node) override {
    if (Node.getLeft())
      Node.getLeft()->accept(*this);
    if (Node.getRight())
      Node.getRight()->accept(*this);
  };

  virtual void visit(UnaryOp &Node) override {
    if (Node.getOperand())
      Node.getOperand()->accept(*this);
  };

  virtual void visit(Assignment &Node) override {
    Final *dest = Node.getLeft();
    if (dest)
      dest->accept(*this);

    Expr *Right = Node.getRight();
    if (Right)
      Right->accept(*this);
  };

  virtual void visit(SpecialAssignment &Node) override {
    // Simple check - just verify variables exist
    if (Scope.find(Node.getDest()) == Scope.end())
      error(Not, Node.getDest());

    if (!Node.getArg1().empty() && Scope.find(Node.getArg1()) == Scope.end())
      error(Not, Node.getArg1());

    if (!Node.getArg2().empty() && Scope.find(Node.getArg2()) == Scope.end())
      error(Not, Node.getArg2());
  };

  virtual void visit(Declaration &Node) override {
    for (llvm::SmallVector<llvm::StringRef, 8>::const_iterator I = Node.varBegin(), E = Node.varEnd(); I != E; ++I) {
      if (!Scope.insert(*I).second)
        error(Twice, *I);
    }
    for (llvm::SmallVector<Expr *, 8>::const_iterator I = Node.valBegin(), E = Node.valEnd(); I != E; ++I){
      (*I)->accept(*this);
    }
  };

  virtual void visit(Comparison &Node) override {
    if(Node.getLeft())
      Node.getLeft()->accept(*this);
    if(Node.getRight())
      Node.getRight()->accept(*this);
  };

  virtual void visit(LogicalExpr &Node) override {
    if(Node.getLeft())
      Node.getLeft()->accept(*this);
    if(Node.getRight())
      Node.getRight()->accept(*this);
  };

  virtual void visit(IfStmt &Node) override {
    if (Node.getCond())
      Node.getCond()->accept(*this);

    for (llvm::SmallVector<AST *, 8>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
    for (llvm::SmallVector<AST *, 8>::const_iterator I = Node.beginElse(), E = Node.endElse(); I != E; ++I){
      (*I)->accept(*this);
    }
  };

  virtual void visit(ForStmt &Node) override {
    if (Node.getInit())
      Node.getInit()->accept(*this);
    if (Node.getCond())
      Node.getCond()->accept(*this);
    if (Node.getIncrement())
      Node.getIncrement()->accept(*this);

    for (llvm::SmallVector<AST *, 8>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
  };

  virtual void visit(ForeachStmt &Node) override {
    // Add loop variable to scope temporarily
    Scope.insert(Node.getVar());

    if (Scope.find(Node.getArray()) == Scope.end())
      error(Not, Node.getArray());

    for (llvm::SmallVector<AST *, 8>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
  };

  virtual void visit(MatchStmt &Node) override {
    if (Node.getValue())
      Node.getValue()->accept(*this);

    for (llvm::SmallVector<MatchCase *, 8>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
  };

  virtual void visit(MatchCase &Node) override {
    if (Node.getPattern())
      Node.getPattern()->accept(*this);

    for (llvm::SmallVector<AST *, 8>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
  };

  virtual void visit(PrintStmt &Node) override {
    if (Node.getValue())
      Node.getValue()->accept(*this);
  };

  virtual void visit(FunctionCall &Node) override {
    for (llvm::SmallVector<Expr *, 4>::const_iterator I = Node.argsBegin(), E = Node.argsEnd(); I != E; ++I) {
      (*I)->accept(*this);
    }
  };

  virtual void visit(ArrayLiteral &Node) override {
    for (llvm::SmallVector<Expr *, 8>::const_iterator I = Node.begin(), E = Node.end(); I != E; ++I) {
      (*I)->accept(*this);
    }
  };

  virtual void visit(ArrayAccess &Node) override {
    if (Scope.find(Node.getArrayName()) == Scope.end())
      error(Not, Node.getArrayName());

    if (Node.getIndex())
      Node.getIndex()->accept(*this);
  };
};
}

bool Sema::semantic(Program *Tree) {
  if (!Tree)
    return false;
  nms::InputCheck *Check = new nms::InputCheck();
  Tree->accept(*Check);
  return Check->hasError();
}
