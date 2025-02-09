#include "gen_main.hpp"

namespace {

int indentation_level = 0;

}

void gen::write(std::ofstream& file, std::string str) {
    file << str;
}

void gen::newline(std::ofstream& file) {
    file << "\n";
    // tabs
    for (int i = 0; i < indentation_level; ++i)
        file << "\t";
}

void gen::indent() {
    ++indentation_level;
}

void gen::unindent() {
    --indentation_level;
    if (indentation_level < 0)
        ZF_BACK_ERR("indentation level went below zero in generated code");
}
