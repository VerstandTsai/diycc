#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

struct ASTNode {
    enum TokenType type;
    char value[MAX_ID_LENGTH];
    struct ASTNode *fisrt_child;
    struct ASTNode *last_child;
    struct ASTNode *next_sibling;
};

struct SyntaxTree {
    struct ASTNode *root;
};

struct ASTNode *new_node(enum TokenType type);
void add_child(struct ASTNode *node, struct ASTNode *child);
void free_tree(struct SyntaxTree tree);
void print_tree(struct SyntaxTree tree);
struct SyntaxTree parse(struct TokenStream tokens);

#endif // PARSER_H

