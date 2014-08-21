#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Parse/ParseDiagnostic.h"
#include "clang/Sema/SemaDiagnostic.h"
#include "clang/Lex/Lexer.h"

using namespace clang;

class RecursiveFunctionDecomposerVisitor
        : public RecursiveASTVisitor<RecursiveFunctionDecomposerVisitor> {
public:
    explicit RecursiveFunctionDecomposerVisitor(ASTContext *Context)
        : Context(Context) {}
    bool VisitDecl(Decl *Declaration)
    {
        if(FunctionDecl *funcDecl = dyn_cast<FunctionDecl>(Declaration))
        {
            llvm::outs() << "function name: " << funcDecl->getNameAsString() << " (return type = " << funcDecl->getResultType().getAsString() << ")\n";
            unsigned paramCount = funcDecl->getNumParams();
            llvm::outs() << "function param count: " << paramCount << "\n";
            for(unsigned i = 0; i < paramCount; ++i)
            {
                llvm::outs() << "-param #" << i << "\n";
                const ParmVarDecl *currentParam = funcDecl->getParamDecl(i);
                QualType userType = currentParam->getType();
                while(userType->isPointerType())
                {
                    llvm::outs() << "\tpointer to" << "\n";
                    userType = userType->getPointeeType();
                }
                if(userType.isConstQualified())
                {
                    llvm::outs() << "\tconst" << "\n";
                }
                if(userType->isReferenceType())
                {
                    llvm::outs() << "\treference to" << "\n";
                }
                userType = userType.getNonReferenceType().getUnqualifiedType();
                llvm::outs() << "\t(type = " << userType.getAsString() << ", name = " << currentParam->getNameAsString() << ")\n";
            }
            llvm::outs() << "\n";
        }
        if(VarDecl *varDecl = dyn_cast<VarDecl>(Declaration))
        {
            if(!dyn_cast<ParmVarDecl>(Declaration)) //parameter variables already output with functions above
            {
                llvm::outs() << "variable type: " << varDecl->getType().getAsString() << ", variable name: " << varDecl->getNameAsString();
                if(varDecl->hasInit())
                {
                    const Expr* varInit = varDecl->getInit();
                    if(varInit->isRValue())
                    {
                        SourceRange varSourceRange = varInit->getSourceRange();
                        if(!varSourceRange.isValid())
                            return true;
                        CharSourceRange charSourceRange(varSourceRange, true);
                        StringRef sourceText = Lexer::getSourceText(charSourceRange, Context->getSourceManager(), Context->getLangOpts(), 0);
                        llvm::outs() << ", initialization value: " << sourceText.str();
                    }
                }
                llvm::outs() << "\n";
            }
        }
        return true;
    }
private:
    ASTContext *Context;
};

class FunctionDecomposerConsumer : public clang::ASTConsumer {
public:
    explicit FunctionDecomposerConsumer(ASTContext *Context)
        : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    RecursiveFunctionDecomposerVisitor Visitor;
};

//error handler
struct FunctionDecomposerDiagnosticConsumer : clang::DiagnosticConsumer {
    llvm::OwningPtr<DiagnosticConsumer> Proxy;
    FunctionDecomposerDiagnosticConsumer(DiagnosticConsumer *Previous) : Proxy(Previous) {}

    void BeginSourceFile(const clang::LangOptions& LangOpts, const clang::Preprocessor* PP = 0) { Proxy->BeginSourceFile(LangOpts, PP); }
    void clear(){ Proxy->clear(); }
    void EndSourceFile() { Proxy->EndSourceFile(); }
    void finish() { Proxy->finish(); }
    void HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel, const clang::Diagnostic& Info) {
        if(DiagLevel == DiagnosticsEngine::Error)
        {
            const unsigned diagId = Info.getID();
            const unsigned numDiagArgs = Info.getNumArgs();
            if(numDiagArgs < 1) return;
            if(diagId == clang::diag::err_unknown_typename)
            {
                llvm::outs() << "Unknown type detected: " << Info.getArgIdentifier(0)->getName().str() << "\n";
            }
            else if(diagId == clang::diag::err_unknown_typename_suggest && Info.getNumFixItHints() > 0)
            {
                llvm::outs() << "Unknown type (with suggestion) detected: " << Info.getArgIdentifier(0)->getName().str() << " (suggestion: " << Info.getFixItHint(0).CodeToInsert << ")\n";
            }
        }
        DiagnosticConsumer::HandleDiagnostic(DiagLevel, Info);
        Proxy->HandleDiagnostic(DiagLevel, Info);
    }
};

class FunctionDecomposerFrontendAction : public clang::ASTFrontendAction {
public:
    virtual clang::ASTConsumer *CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile)
    {
        Compiler.getDiagnostics().setClient(new FunctionDecomposerDiagnosticConsumer(Compiler.getDiagnostics().takeClient()));
        return new FunctionDecomposerConsumer(&Compiler.getASTContext());
    }
};

int main(int argc, char **argv) {
    clang::tooling::runToolOnCode(new FunctionDecomposerFrontendAction, ((argc > 1) ? argv[1] : "int someFunc(int x, char **y); bool b; int x = 0; int y(x); inz /*typo*/ hi = y;"));
}
