//--------------------------------------------------------------------*- C++ -//
// clad - the C++ Clang-based Automatic Differentiator
//
// A constant folding tool, working on AST level
//
// author:  Vassil Vassilev <vvasilev-at-cern.ch>
//----------------------------------------------------------------------------//

#ifndef CLAD_CONSTANT_FOLDER_H
#define CLAD_CONSTANT_FOLDER_H

#include "clang/AST/StmtVisitor.h"

namespace clang {
  class ASTContext;
  class BinaryOperator;
  class Expr;
  class QualType;
  class Stmt;
}

namespace clad {
  class ConstantFolder: public clang::StmtVisitor<ConstantFolder, clang::Stmt*> {
  private:
    clang::ASTContext& m_Context;
    bool m_Enabled;
  public:
    ConstantFolder(clang::ASTContext& C) : m_Context(C), m_Enabled(false) {}
    clang::Expr* fold(clang::Expr* E);
    clang::Stmt* VisitExpr(clang::Expr* E);
    static clang::Expr* synthesizeLiteral(clang::QualType, clang::ASTContext &C,
                                          uint64_t val);
  };
} // end namespace clad
#endif // CLAD_CONSTANT_FOLDER_H
