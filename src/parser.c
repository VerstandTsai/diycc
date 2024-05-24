#include "parser.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ASTNode *new_node(enum TokenType type) {
    struct ASTNode *node = (struct ASTNode*)malloc(sizeof(struct ASTNode));
    node->type = type;
    node->value[0] = '\0';
    node->fisrt_child = NULL;
    node->last_child = NULL;
    node->next_sibling = NULL;
    return node;
}

void add_child(struct ASTNode *node, struct ASTNode *child) {
    if (!node->fisrt_child) {
        node->fisrt_child = child;
        node->last_child = child;
        return;
    }
    node->last_child->next_sibling = child;
    node->last_child = node->last_child->next_sibling;
}

void free_tree(struct SyntaxTree tree) {
    struct ASTNode *current = tree.root->fisrt_child;
    while (current) {
        struct ASTNode *temp = current->next_sibling;
        struct SyntaxTree subtree;
        subtree.root = current;
        free_tree(subtree);
        current = temp;
    }
    free(tree.root);
}

void print_tree(struct SyntaxTree tree) {
    struct ASTNode *current = tree.root->fisrt_child;
    if (!current) return;
    printf("[%d, %s] -> ", tree.root->type, tree.root->value);
    struct ASTNode *children[32];
    int num_child = 0;
    while (current) {
        printf("[%d, %s], ", current->type, current->value);
        children[num_child++] = current;
        current = current->next_sibling;
    }
    printf("\n");
    for (int i=0; i<num_child; i++) {
        struct SyntaxTree temp;
        temp.root = children[i];
        print_tree(temp);
    }
}

int isdelim(enum TokenType type) {
    switch (type) {
    case TK_SEMICOLON:
    case TK_LPAREN:
    case TK_RPAREN:
    case TK_LBRACE:
    case TK_RBRACE:
        return 1;
    default:
        return 0;
    }
}

struct ASTNode *match(enum TokenType type, struct Token **token_ptr) {
    if (!*token_ptr) return NULL;
    enum TokenType basic_types[] = {
        TK_INT
    };
    enum TokenType funcseq[] = {
        TK_TYPESPEC,
        TK_ID,
        TK_LPAREN,
        TK_RPAREN,
        TK_LBRACE,
        TK_BODY,
        TK_RBRACE
    };
    enum TokenType statement_types[] = {
        TK_VARDEC,
        TK_VARINIT,
        TK_ASSIGNMENT,
        TK_RETST
    };
    enum TokenType vardecseq[] = {
        TK_TYPESPEC,
        TK_ID,
        TK_SEMICOLON
    };
    enum TokenType varinitseq[] = {
        TK_TYPESPEC,
        TK_ID,
        TK_ASSIGN,
        TK_EXP,
        TK_SEMICOLON
    };
    enum TokenType assseq[] = {
        TK_VAR,
        TK_ASSOP,
        TK_EXP,
        TK_SEMICOLON
    };
    enum TokenType retstseq[] = {
        TK_RETURN,
        TK_EXP,
        TK_SEMICOLON
    };
    enum TokenType assops[] = {
        TK_ASSIGN
    };
    enum TokenType var_types[] = {
        TK_ID
    };
    enum TokenType exp_types[] = {
        TK_VAR,
        TK_NUMBER
    };
    struct ASTNode *root;
    struct ASTNode *child;
    struct SyntaxTree temp_tree; // Tree for calling free_tree
    struct Token *token_ptr_backup = *token_ptr;
    switch (type) {
    case TK_PROGRAM:
        root = new_node(TK_PROGRAM);
        while (1) {
            child = match(TK_FUNCDEF, token_ptr);
            if (!child) break;
            add_child(root, child);
        }
        return root;
    case TK_FUNCDEF:
        root = new_node(TK_FUNCDEF);
        for (int i=0; i<7; i++) {
            child = match(funcseq[i], token_ptr);
            if (!child) {
                *token_ptr = token_ptr_backup;
                temp_tree.root = root;
                free_tree(temp_tree);
                return NULL;
            }
            if (isdelim(funcseq[i])) continue;
            add_child(root, child);
        }
        return root;
    case TK_TYPESPEC:;
        for (int i=0; i<1; i++) {
            root = match(basic_types[i], token_ptr);
            if (root) return root;
        }
        return NULL;
    case TK_BODY:
        root = new_node(TK_BODY);
        while (1) {
            child = match(TK_STATEMENT, token_ptr);
            if (!child) break;
            add_child(root, child);
        }
        return root;
    case TK_STATEMENT:
        for (int i=0; i<4; i++) {
            root = match(statement_types[i], token_ptr);
            if (root) return root;
        }
        return NULL;
    case TK_VARDEC:
        root = new_node(TK_VARDEC);
        for (int i=0; i<3; i++) {
            child = match(vardecseq[i], token_ptr);
            if (!child) {
                *token_ptr = token_ptr_backup;
                temp_tree.root = root;
                free_tree(temp_tree);
                return NULL;
            }
            if (isdelim(vardecseq[i])) continue;
            add_child(root, child);
        }
        return root;
    case TK_VARINIT:
        root = new_node(TK_VARINIT);
        for (int i=0; i<5; i++) {
            child = match(varinitseq[i], token_ptr);
            if (!child) {
                *token_ptr = token_ptr_backup;
                temp_tree.root = root;
                free_tree(temp_tree);
                return NULL;
            }
            if (isdelim(varinitseq[i]) || varinitseq[i] == TK_ASSIGN) continue;
            add_child(root, child);
        }
        return root;
    case TK_VAR:
        for (int i=0; i<1; i++) {
            root = match(var_types[i], token_ptr);
            if (root) return root;
        }
        return NULL;
    case TK_ASSIGNMENT:
        root = new_node(TK_ASSIGNMENT);
        for (int i=0; i<4; i++) {
            child = match(assseq[i], token_ptr);
            if (!child) {
                *token_ptr = token_ptr_backup;
                temp_tree.root = root;
                free_tree(temp_tree);
                return NULL;
            }
            if (isdelim(assseq[i])) continue;
            add_child(root, child);
        }
        return root;
    case TK_ASSOP:
        for (int i=0; i<1; i++) {
            root = match(assops[i], token_ptr);
            if (root) return root;
        }
        return NULL;
    case TK_EXP:
        for (int i=0; i<3; i++) {
            root = match(exp_types[i], token_ptr);
            if (root) return root;
        }
        return NULL;
    case TK_RETST:
        root = new_node(TK_RETST);
        for (int i=0; i<3; i++) {
            child = match(retstseq[i], token_ptr);
            if (!child) {
                *token_ptr = token_ptr_backup;
                temp_tree.root = root;
                free_tree(temp_tree);
                return NULL;
            }
            if (isdelim(retstseq[i]) || retstseq[i] == TK_RETURN) continue;
            add_child(root, child);
        }
        return root;
    case TK_ID:
    case TK_NUMBER:
    case TK_INT:
    case TK_RETURN:
    case TK_ASSIGN:
    case TK_LPAREN:
    case TK_RPAREN:
    case TK_LBRACE:
    case TK_RBRACE:
    case TK_SEMICOLON:
        if ((*token_ptr)->type != type) return NULL;
        root = new_node(type);
        strcpy(root->value, (*token_ptr)->value);
        (*token_ptr) = (*token_ptr)->next;
        return root;
    default:
        return NULL;
    }
}

struct SyntaxTree parse(struct TokenStream tokens) {
    struct SyntaxTree tree;
    tree.root = match(TK_PROGRAM, &tokens.head);
    return tree;
}

