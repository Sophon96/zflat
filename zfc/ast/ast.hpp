/**
 * AST - the parent class of all syntax components.
 */

#pragma once

#include <fstream>
#include <vector>
#include <string>

#include <back/sym/symtab.hpp>
#include <front/trfrags.hpp>
#include <util/error.hpp>

#define ZF_TOK_ERR(tok, exp_name) ZF_ERROR("expected " exp_name " on line %d, found \"%s\" instead", tok.line, tok.raw_content())

struct ASTNode {

    int line = -1;

    virtual TreeComp type() = 0;

    virtual void read(std::ifstream& file) = 0;
    virtual void write(std::ofstream& file) = 0;
    virtual void validate();

};

struct ProgramNode;
struct ProgramSub;
struct FunctionNode;
struct StatementNode;
struct BlockStatementNode;
struct ExprNode;
struct FuncCallNode;
struct ControlFlowNode;
struct VarDeclNode;

struct ProgramNode : public ASTNode {
    std::vector<ProgramSub*> components;
    TreeComp type() override;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    virtual ~ProgramNode();
    void validate() override;
};

struct ProgramSub : public ASTNode {
    virtual ~ProgramSub();
};

struct FunctionNode : public ProgramSub {
    std::string name;
    std::vector<VarDeclNode*> args;
    std::string ret_type;
    BlockStatementNode* body;
    TreeComp type() override;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    virtual ~FunctionNode();
    void validate() override;
};

struct InnerStatementNode : public ProgramSub {
    TreeComp type() override;
    virtual ~InnerStatementNode() = 0;
};

struct StatementNode : public ProgramSub {
    InnerStatementNode* inner;
    TreeComp type() override;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    virtual ~StatementNode();
    void validate() override;
};

struct BlockStatementNode : public InnerStatementNode {
    std::vector<StatementNode*> statements;
    TreeComp type() override;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    virtual ~BlockStatementNode();
    sym::SymbolTable* table; // the symbol table associated with this block
    void build_symtab();
    void validate() override;
};

struct ExprNode : public InnerStatementNode {
    std::string literal;
    ExprNode* left;
    std::string op;
    ExprNode* right;
    TreeComp type() override;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    virtual ~ExprNode();
    std::string get_type();
    void validate() override;
};

struct FuncCallNode : public ExprNode {
    std::vector<ExprNode*> args;
    std::string name;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    virtual ~FuncCallNode();
    void validate() override;
};

struct ControlFlowNode : public InnerStatementNode {
    std::string statement;
    ExprNode* expression;
    TreeComp type() override;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    virtual ~ControlFlowNode();
    void validate() override;
};

struct VarDeclNode : public InnerStatementNode {
    std::string ntype;
    std::string name;
    ExprNode* expr;
    TreeComp type() override;
    void read(std::ifstream& file) override;
    void write(std::ofstream& file) override;
    virtual ~VarDeclNode();
    void validate() override;
};
