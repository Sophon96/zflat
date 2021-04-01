#include "lex.hpp"
#include <cctype>

Token::Token() {
    
}

Token::Token(TreeComp t, std::string s)
: type(t), str(s) {

}

const char* Token::raw_content() {
    return this->str.c_str();
}

namespace {

// Whether in the INSIDE of a comment or a quote.
bool in_comment = false;

bool is_op_token(char c) {
    switch (c) {
        case '!':
        case '%':
        case '^':
        case '&':
        case '*':
        case '-':
        case '+':
        case '|':
        case '/':
        case '<':
        case '>':
        case '=':
            return true;
        default:
            return false;
    }
}

bool is_always_tok(char c) {
    switch (c) {
        case ':':
        case ';':
        case '[':
        case ']':
        case '{':
        case '}':
        case '(':
        case ')':
        case ',':
        case '.':
            return true;
        default:
            return false;
    }
}

const int TOK_MAX = 256;

char cbuf[TOK_MAX + 1];
int cend = 0;

void add_char(char c) {
    cbuf[cend++] = c;
}

bool begins_token(char prev, char cur) {
    if (cur == '~')                 return (!in_comment);
    if (in_comment)                 return false;
    if (isspace(cur))         return false;
    if (isspace(prev))        return true;
    if (cend == 0)                  return true;
    if (prev == '\0')               return true;
    if (is_op_token(cur))           return true;
    if (is_always_tok(cur))         return true;
    if (is_always_tok(prev))         return true;
    return false;
}

bool ends_token(char cur, char next) {
    if (cur == '~')                 return in_comment;
    if (in_comment)                 return false;
    if (isspace(cur))         return false;
    if (is_space(next))        return true;
    if (next < 1)                   return true;
    if (is_op_token(cur))           return (next != '=');
    if (is_always_tok(cur))         return true;
    if (is_always_tok(next))        return true;
    return false;
}

char lasttok = 0, curtok = 0, nexttok = 0;

bool eofhit = false;

bool lastwascolon = false;

int line = 1, linepos = 1;

TreeComp get_type(std::string name) {
    if (curtok == EOF) return TreeComp::TEOF;
    switch (curtok) {
        case '~':
            return TreeComp::COMMENT;   
        case '!':
        case '%':
        case '^':
        case '&':
        case '*':
        case '-':
        case '+':
        case '|':
        case '/':
        case '<':
        case '>':
        case '=':
            return TreeComp::OPERATOR;
        case ':':
            return TreeComp::COLON;
        case ';':
            return TreeComp::SEMICOLON;
        case '[':
            return TreeComp::OBRACKET;
        case ']':
            return TreeComp::CBRACKET;
        case '{':
            return TreeComp::OBRACE;
        case '}':
            return TreeComp::CBRACE;
        case '(':
            return TreeComp::OPAREN;
        case ')':
            return TreeComp::CPAREN;
        case ',':
            return TreeComp::COMMA;
        case '.':
            return TreeComp::DOT;
        default:;
    }
    if (name == "break" || name == "continue" || name == "return")
        return TreeComp::FLOW;
    if (
        name == "int" || name == "double" || name == "float" || name == "char"
    ||  name == "short" || name == "long" || name == "bool" || name == "void"
    ) return TreeComp::TYPENAME;
    if (name == "loop") return TreeComp::LOOP;
    if (name == "if") return TreeComp::IF;
    if (name == "else") return TreeComp::ELSE;
    if (isalpha(name[0])) {
        return (lastwascolon) ? TreeComp::TYPENAME : TreeComp::IDENTIFIER;
    }
    if (isdigit(name[0]))
        return TreeComp::LITERAL;
    // when in doubt ...
    return TreeComp::IDENTIFIER;
}

std::vector<Token> put_back_tokens;

Token process_character(std::ifstream& file) {
    if (eofhit) {
        if (in_comment) ZF_ERROR("Open comment");
        return Token(TreeComp::TEOF, "[end of file]");
    }
    lasttok = curtok;
    curtok = file.get();
    nexttok = file.peek();
    if (curtok == '\n') {
        ++line;
        linepos = 1;
    } else {
        ++linepos;
    }
    if (cend >= TOK_MAX)
        ZF_ERROR("token on line %u exceeded limit", line);
    if (ends_token(curtok, nexttok)) {
        lastwascolon = curtok == ':';
        add_char(curtok);
        Token tok(get_type(cbuf), cbuf);
        cend = 0;
        memset(cbuf, 0, sizeof(char) * TOK_MAX + 1);
        if (curtok == EOF) {
            eofhit = true;
        } else if (curtok == '~') {
            in_comment = false;
        }
        return tok;
    } else {
        if (begins_token(lasttok, curtok)) {
            cend = 0;
        }
        if (curtok == '~') {
            in_comment = !in_comment;
        }
        if (!is_whitespace(curtok) && !in_comment) {
            add_char(curtok);
        }
    }
    return Token(TreeComp::EGENERALERROR, "");
}

Token inner_get_token(std::ifstream& file) {
    Token ret;
    while ( (ret = process_character(file)).type == TreeComp::EGENERALERROR );
    ret.line = line;
    ret.linepos = linepos;
    return ret;
}

}

Token lex::lex(std::ifstream& file) {
    Token tok;
    if (put_back_tokens.size() > 0) {
        tok = put_back_tokens[put_back_tokens.size() - 1];
        put_back_tokens.pop_back();
    }
    else
        while ( (tok = inner_get_token(file)).type == TreeComp::COMMENT );
    return tok;
}

void lex::unlex(Token token) {
    put_back_tokens.push_back(token);
}
