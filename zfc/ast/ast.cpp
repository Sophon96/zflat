#include "ast.hpp"

namespace {
void pdel(ASTNode* node) { if (dynamic_cast<ProgramNode*>(node) == nullptr) delete node; }
}

ProgramNode::~ProgramNode() {
    this->apply(pdel);
}

void ProgramNode::apply(void(*fn)(ASTNode*)) {
    for (auto sub : this->components) {
        fn(sub);
    }
    fn(this);
}

void FuncCallNode::apply(void(*fn)(ASTNode*)) {
    for (auto arg : this->args) {
        fn(arg);
    }
    fn(this);
}

void FunctionNode::apply(void(*fn)(ASTNode*)) {
    for (auto decl : this->args) {
        fn(decl);
    }
    fn(this->body);
    fn(this);
}

void StatementNode::apply(void(*fn)(ASTNode*)) {
    fn(this->inner);
    fn(this);
}

void BlockStatementNode::apply(void(*fn)(ASTNode*)) {
    for (auto stmt : this->statements) {
        fn(stmt);
    }
    fn(this);
}

void IfNode::apply(void(*fn)(ASTNode*)) {
    fn(this->expr);
    fn(this->stmt);
    if (this->else_block != nullptr) fn(this->else_block);
    fn(this);
}

void LoopNode::apply(void(*fn)(ASTNode*)) {
    fn(this->expr);
    if (this->pred != nullptr) fn(this->pred);
    fn(this->stmt);
    fn(this);
}

void ExprNode::apply(void(*fn)(ASTNode*)) {
    if (this->left != nullptr) fn(this->left);
    if (this->right != nullptr) fn(this->right);
    fn(this);
}

void ControlFlowNode::apply(void(*fn)(ASTNode*)) {
    if (this->expression != nullptr) fn(this->expression);
    fn(this);
}

void VarDeclNode::apply(void(*fn)(ASTNode*)) {
    if (this->expr != nullptr) fn(this->expr);
    fn(this);
}
