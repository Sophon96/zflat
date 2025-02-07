#include <ast/ast.hpp>
#include <back/gen/gen_main.hpp>

void StatementNode::write(std::ofstream& file) {

    gen::newline(file);
    this->inner->write(file);
    if (
        dynamic_cast<BlockStatementNode*> (this->inner) == nullptr
        && dynamic_cast<IfNode*> (this->inner) == nullptr
        && dynamic_cast<LoopNode*> (this->inner) == nullptr
    )
        gen::write(file, ";");

}
