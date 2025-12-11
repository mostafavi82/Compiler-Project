#include "CodeGen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  class ToIRVisitor : public ASTVisitor {
    Module *M;
    IRBuilder<> Builder;
    Type *VoidTy;
    Type *Int32Ty;
    Type *Int8PtrTy;
    Constant *Int32Zero;

    Value *V;
    StringMap<Value *> nameMap;

    Function *PrintfFn;

  public:
    ToIRVisitor(Module *M) : M(M), Builder(M->getContext()) {
      VoidTy = Type::getVoidTy(M->getContext());
      Int32Ty = Type::getInt32Ty(M->getContext());
      Int8PtrTy = Type::getInt8PtrTy(M->getContext());
      Int32Zero = ConstantInt::get(Int32Ty, 0, true);

      // Declare printf
      FunctionType *PrintfTy = FunctionType::get(Int32Ty, {Int8PtrTy}, true);
      PrintfFn = Function::Create(PrintfTy, GlobalValue::ExternalLinkage, "printf", M);
    }

    void run(Program *Tree) {
      FunctionType *MainFty = FunctionType::get(Int32Ty, {}, false);
      Function *MainFn = Function::Create(MainFty, GlobalValue::ExternalLinkage, "main", M);
      BasicBlock *BB = BasicBlock::Create(M->getContext(), "entry", MainFn);
      Builder.SetInsertPoint(BB);

      Tree->accept(*this);

      Builder.CreateRet(Int32Zero);
    }

    virtual void visit(Program &Node) override {
      for (auto I = Node.begin(), E = Node.end(); I != E; ++I) {
        (*I)->accept(*this);
      }
    }

    virtual void visit(Declaration &Node) override {
      auto VarIt = Node.varBegin();
      auto ValIt = Node.valBegin();

      for (; VarIt != Node.varEnd(); ++VarIt) {
        AllocaInst *Alloca = Builder.CreateAlloca(Int32Ty, nullptr, *VarIt);
        nameMap[*VarIt] = Alloca;

        if (ValIt != Node.valEnd()) {
          (*ValIt)->accept(*this);
          Builder.CreateStore(V, Alloca);
          ++ValIt;
        } else {
          Builder.CreateStore(Int32Zero, Alloca);
        }
      }
    }

    virtual void visit(Assignment &Node) override {
      Node.getRight()->accept(*this);
      Value *RightVal = V;

      StringRef VarName = Node.getLeft()->getVal();
      Value *Var = nameMap[VarName];

      if (Node.getAssignKind() != Assignment::Assign) {
        Value *OldVal = Builder.CreateLoad(Int32Ty, Var);
        switch (Node.getAssignKind()) {
          case Assignment::Plus_assign:
            RightVal = Builder.CreateAdd(OldVal, RightVal);
            break;
          case Assignment::Minus_assign:
            RightVal = Builder.CreateSub(OldVal, RightVal);
            break;
          case Assignment::Star_assign:
            RightVal = Builder.CreateMul(OldVal, RightVal);
            break;
          case Assignment::Slash_assign:
            RightVal = Builder.CreateSDiv(OldVal, RightVal);
            break;
          default:
            break;
        }
      }

      Builder.CreateStore(RightVal, Var);
    }

    virtual void visit(SpecialAssignment &Node) override {
      Value *Dest = nameMap[Node.getDest()];

      if (Node.getOpKind() == SpecialAssignment::INC) {
        Value *OldVal = Builder.CreateLoad(Int32Ty, Dest);
        Value *NewVal = Builder.CreateAdd(OldVal, ConstantInt::get(Int32Ty, 1));
        Builder.CreateStore(NewVal, Dest);
      }
      else if (Node.getOpKind() == SpecialAssignment::DEC) {
        Value *OldVal = Builder.CreateLoad(Int32Ty, Dest);
        Value *NewVal = Builder.CreateSub(OldVal, ConstantInt::get(Int32Ty, 1));
        Builder.CreateStore(NewVal, Dest);
      }
      else {
        // For ADD, SUB, etc. - simplified implementation
        Value *Arg1Val = Builder.CreateLoad(Int32Ty, nameMap[Node.getArg1()]);
        Value *Arg2Val = !Node.getArg2().empty() ? Builder.CreateLoad(Int32Ty, nameMap[Node.getArg2()]) : nullptr;
        Value *Result = nullptr;

        switch (Node.getOpKind()) {
          case SpecialAssignment::ADD:
            Result = Builder.CreateAdd(Arg1Val, Arg2Val);
            break;
          case SpecialAssignment::SUB:
            Result = Builder.CreateSub(Arg1Val, Arg2Val);
            break;
          case SpecialAssignment::MUL:
            Result = Builder.CreateMul(Arg1Val, Arg2Val);
            break;
          case SpecialAssignment::DIV:
            Result = Builder.CreateSDiv(Arg1Val, Arg2Val);
            break;
          case SpecialAssignment::PLE:
            Result = Builder.CreateAdd(Builder.CreateLoad(Int32Ty, Dest), Arg1Val);
            break;
          case SpecialAssignment::MIE:
            Result = Builder.CreateSub(Builder.CreateLoad(Int32Ty, Dest), Arg1Val);
            break;
          default:
            Result = Arg1Val;
            break;
        }

        if (Result)
          Builder.CreateStore(Result, Dest);
      }
    }

    virtual void visit(Final &Node) override {
      if (Node.getKind() == Final::Ident) {
        V = Builder.CreateLoad(Int32Ty, nameMap[Node.getVal()]);
      } else if (Node.getKind() == Final::Bool) {
        int val = (Node.getVal() == "true") ? 1 : 0;
        V = ConstantInt::get(Int32Ty, val);
      } else {
        int intval;
        Node.getVal().getAsInteger(10, intval);
        V = ConstantInt::get(Int32Ty, intval, true);
      }
    }

    virtual void visit(BinaryOp &Node) override {
      Node.getLeft()->accept(*this);
      Value *Left = V;
      Node.getRight()->accept(*this);
      Value *Right = V;

      switch (Node.getOperator()) {
        case BinaryOp::Plus:
          V = Builder.CreateAdd(Left, Right);
          break;
        case BinaryOp::Minus:
          V = Builder.CreateSub(Left, Right);
          break;
        case BinaryOp::Mul:
          V = Builder.CreateMul(Left, Right);
          break;
        case BinaryOp::Div:
          V = Builder.CreateSDiv(Left, Right);
          break;
        case BinaryOp::Mod:
          V = Builder.CreateSRem(Left, Right);
          break;
        default:
          V = Left;
          break;
      }
    }

    virtual void visit(UnaryOp &Node) override {
      Node.getOperand()->accept(*this);
    }

    virtual void visit(Comparison &Node) override {
      Node.getLeft()->accept(*this);
      Value *Left = V;
      Node.getRight()->accept(*this);
      Value *Right = V;

      switch (Node.getOperator()) {
        case Comparison::Equal:
          V = Builder.CreateICmpEQ(Left, Right);
          break;
        case Comparison::Not_equal:
          V = Builder.CreateICmpNE(Left, Right);
          break;
        case Comparison::Greater:
          V = Builder.CreateICmpSGT(Left, Right);
          break;
        case Comparison::Less:
          V = Builder.CreateICmpSLT(Left, Right);
          break;
        case Comparison::Greater_equal:
          V = Builder.CreateICmpSGE(Left, Right);
          break;
        case Comparison::Less_equal:
          V = Builder.CreateICmpSLE(Left, Right);
          break;
      }
    }

    virtual void visit(LogicalExpr &Node) override {
      Node.getLeft()->accept(*this);
      Value *Left = V;
      Node.getRight()->accept(*this);
      Value *Right = V;

      if (Node.getOperator() == LogicalExpr::And)
        V = Builder.CreateAnd(Left, Right);
      else
        V = Builder.CreateOr(Left, Right);
    }

    virtual void visit(IfStmt &Node) override {
      Node.getCond()->accept(*this);
      Value *Cond = V;

      Function *TheFunction = Builder.GetInsertBlock()->getParent();
      BasicBlock *ThenBB = BasicBlock::Create(M->getContext(), "then", TheFunction);
      BasicBlock *ElseBB = BasicBlock::Create(M->getContext(), "else");
      BasicBlock *MergeBB = BasicBlock::Create(M->getContext(), "ifcont");

      Builder.CreateCondBr(Cond, ThenBB, ElseBB);

      Builder.SetInsertPoint(ThenBB);
      for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
        (*I)->accept(*this);
      Builder.CreateBr(MergeBB);

      TheFunction->getBasicBlockList().push_back(ElseBB);
      Builder.SetInsertPoint(ElseBB);
      for (auto I = Node.beginElse(), E = Node.endElse(); I != E; ++I)
        (*I)->accept(*this);
      Builder.CreateBr(MergeBB);

      TheFunction->getBasicBlockList().push_back(MergeBB);
      Builder.SetInsertPoint(MergeBB);
    }

    virtual void visit(ForStmt &Node) override {
      // Simplified - just visit statements
      if (Node.getInit())
        Node.getInit()->accept(*this);

      Function *TheFunction = Builder.GetInsertBlock()->getParent();
      BasicBlock *LoopBB = BasicBlock::Create(M->getContext(), "loop", TheFunction);
      BasicBlock *AfterBB = BasicBlock::Create(M->getContext(), "afterloop");

      Builder.CreateBr(LoopBB);
      Builder.SetInsertPoint(LoopBB);

      for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
        (*I)->accept(*this);

      if (Node.getIncrement())
        Node.getIncrement()->accept(*this);

      if (Node.getCond()) {
        Node.getCond()->accept(*this);
        Builder.CreateCondBr(V, LoopBB, AfterBB);
      } else {
        Builder.CreateBr(LoopBB);
      }

      TheFunction->getBasicBlockList().push_back(AfterBB);
      Builder.SetInsertPoint(AfterBB);
    }

    virtual void visit(ForeachStmt &Node) override {
      // Placeholder - simplified
      for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
        (*I)->accept(*this);
    }

    virtual void visit(MatchStmt &Node) override {
      // Placeholder - simplified
      for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
        (*I)->accept(*this);
    }

    virtual void visit(MatchCase &Node) override {
      for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
        (*I)->accept(*this);
    }

    virtual void visit(PrintStmt &Node) override {
      Node.getValue()->accept(*this);
      Value *Val = V;

      // Create format string
      Value *FormatStr = Builder.CreateGlobalStringPtr("%d\n");
      Builder.CreateCall(PrintfFn, {FormatStr, Val});
    }

    virtual void visit(FunctionCall &Node) override {
      // Placeholder - simplified
      V = Int32Zero;
    }

    virtual void visit(ArrayLiteral &Node) override {
      // Placeholder - simplified
      V = Int32Zero;
    }

    virtual void visit(ArrayAccess &Node) override {
      // Placeholder - simplified
      V = Int32Zero;
    }
  };
}

void CodeGen::compile(Program *Tree) {
  LLVMContext Ctx;
  Module *M = new Module("simple-compiler", Ctx);

  ToIRVisitor *ToIR = new ToIRVisitor(M);
  ToIR->run(Tree);

  M->print(outs(), nullptr);
}
