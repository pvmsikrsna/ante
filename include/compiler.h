#ifndef COMPILER_H
#define COMPILER_H

#include <climits> //required by llvm is using clang
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <memory>
#include <stack>
#include <map>

using namespace llvm;
using namespace std;

/* Forward-declarations of Nodes defined in parser.h */
struct Node;
struct VarNode;
struct BinOpNode;
struct FuncDeclNode;
struct FuncCallNode;
struct StrLitNode;
struct IntLitNode;

namespace ante{
    struct Compiler{
        unique_ptr<legacy::FunctionPassManager> passManager;
        unique_ptr<Module> module;
        unique_ptr<Node> ast;
        IRBuilder<> builder;

        //Stack of maps of variables mapped to their identifier.
        //Maps are seperated according to their scope.
        stack<std::map<string, Value*>> varTable;

        //Map of declared, but non-defined functions
        map<string, FuncDeclNode*> fnDecls;

        bool errFlag, compiled;
        string fileName;
        
        Compiler(char *fileName);
        ~Compiler();

        void compile();
        void compileNative();
        void compilePrelude();
        void emitIR();
        void enterNewScope();
        void exitScope();
        
        Value* compAdd(Type *t, Value *l, Value *r, BinOpNode *op);
        Value* compSub(Type *t, Value *l, Value *r, BinOpNode *op);
        Value* compMul(Type *t, Value *l, Value *r, BinOpNode *op);
        Value* compDiv(Type *t, Value *l, Value *r, BinOpNode *op);
        Value* compRem(Type *t, Value *l, Value *r, BinOpNode *op);
        
        Value* compErr(string msg, unsigned int row, unsigned int col);

        Function* compFn(FuncDeclNode *fn);
        void registerFunction(FuncDeclNode *func);

        static Type* tokTypeToLlvmType(int tokTy, string typeName);
        static int llvmTypeToTokType(Type *t);

        Value* lookup(string var);
        void stoVar(string var, Value *val);

        void checkIntSize(Value **lhs, Value **rhs);
        
        static int compileIRtoObj(Module *m, string inFile, string outFile);
        static int linkObj(string inFiles, string outFile);
    };
}

#endif
