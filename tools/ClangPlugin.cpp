//--------------------------------------------------------------------*- C++ -*-
// clad - the C++ Clang-based Automatic Differentiator
// version: $Id$
// author:  Vassil Vassilev <vvasilev-at-cern.ch>
//------------------------------------------------------------------------------

#include "ClangPlugin.h"

#include "clad/Differentiator/DerivativeBuilder.h"
#include "clad/Differentiator/Version.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Version.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/MultiplexConsumer.h"
#include "clang/Sema/Sema.h"

#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace clad {
  namespace plugin {
    CladPlugin::CladPlugin(CompilerInstance& CI, DifferentiationOptions& DO)
      : m_CI(CI), m_DO(DO) { }
    CladPlugin::~CladPlugin() {}

    void CladPlugin::Initialize(ASTContext& Context) {
      // We need to reorder the consumers in the MultiplexConsumer.
      MultiplexConsumer& multiplex
        = static_cast<MultiplexConsumer&>(m_CI.getASTConsumer());
      std::vector<ASTConsumer*>& consumers = multiplex.getConsumers();
      ASTConsumer* lastConsumer = consumers.back();
      consumers.pop_back();
      consumers.insert(consumers.begin(), lastConsumer);
    }

    bool CladPlugin::HandleTopLevelDecl(DeclGroupRef DGR) {
      if (!m_DerivativeBuilder)
        m_DerivativeBuilder.reset(new DerivativeBuilder(m_CI.getSema()));

      DiffPlans plans;
      // Instantiate all pending for instantiations templates, because we will
      // need the full bodies to produce derivatives.
      m_CI.getSema().PerformPendingInstantiations();
      DiffCollector collector(DGR, plans, m_CI.getSema());

      //set up printing policy
      clang::LangOptions LangOpts;
      LangOpts.CPlusPlus = true;
      clang::PrintingPolicy Policy(LangOpts);
      for (DiffPlans::iterator plan = plans.begin(), planE = plans.end();
           plan != planE; ++plan)
         for (DiffPlan::iterator I = plan->begin(), E = plan->end();
              I != E; ++I) {
            if (!I->isValid())
               continue;
            // if enabled, print source code of the original functions
            if (m_DO.DumpSourceFn) {
               I->getFD()->print(llvm::outs(), Policy);
            }
            // if enabled, print ASTs of the original functions
            if (m_DO.DumpSourceFnAST) {
               I->getFD()->dumpColor();
            }

            // derive the collected functions
            FunctionDecl* Derivative
               = m_DerivativeBuilder->Derive(I->getFD(), I->getPVD());
            if (I + 1 == E) // The last element
               plan->updateCall(Derivative, m_CI.getSema());

            // if enabled, print source code of the derived functions
            if (m_DO.DumpDerivedFn) {
               Derivative->print(llvm::outs(), Policy);
            }
            // if enabled, print ASTs of the derived functions
            if (m_DO.DumpDerivedAST) {
               Derivative->dumpColor();
            }
            // if enabled, print the derivatives in a file.
            if (m_DO.GenerateSourceFile) {
               std::string err;
               llvm::raw_fd_ostream f("Derivatives.cpp", err,
                                      llvm::sys::fs::F_Append);
               Derivative->print(f, Policy);
               f.flush();
            }
            if (Derivative) {
               Derivative->getDeclContext()->addDecl(Derivative);
               // Call CodeGen only if the produced decl is a top-most decl.
               if (Derivative->getDeclContext()
                   == m_CI.getASTContext().getTranslationUnitDecl())
                  m_CI.getASTConsumer().HandleTopLevelDecl(DeclGroupRef(Derivative));
            }
      }
      return true; // Happiness
    }
  } // end namespace plugin
} // end namespace clad

using namespace clad::plugin;
// register the PluginASTAction in the registry.
static clang::FrontendPluginRegistry::Add<Action<CladPlugin> >
X("clad","Produces derivatives or arbitrary functions");
