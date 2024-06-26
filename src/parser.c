#include "parser.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum TokenType prog_repcands[] = {
    TK_FUNCDEF,
    END_REPCANDS
};

enum TokenType block_repcands[] = {
    TK_STATEMENT,
    END_REPCANDS
};

enum TokenType *repcands_table[] = {
    prog_repcands,
    block_repcands
};

enum TokenType typespec_cands[] = {
    TK_INT,
    END_CANDS
};

enum TokenType statement_cands[] = {
    TK_EXPRST,
    TK_VARDEC,
    TK_VARINIT,
    TK_RETST,
    END_CANDS
};

enum TokenType expr_cands[] = {
    TK_ASSIGNMENT,
    TK_VAR,
    TK_NUMBER,
    END_CANDS
};

enum TokenType var_cands[] = {
    TK_ID,
    END_CANDS
};

enum TokenType assop_cands[] = {
    TK_ASSIGN,
    END_CANDS
};

enum TokenType *cand_table[] = {
    typespec_cands,
    statement_cands,
    expr_cands,
    var_cands,
    assop_cands
};

enum TokenType funcseq[] = {
    TK_TYPESPEC,
    TK_ID,
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_BLOCK,
    TK_RBRACE,
    END_SEQ
};

enum TokenType exprstseq[] = {
    TK_EXPR,
    TK_SEMICOLON,
    END_SEQ
};

enum TokenType vardecseq[] = {
    TK_TYPESPEC,
    TK_ID,
    TK_SEMICOLON,
    END_SEQ
};

enum TokenType varinitseq[] = {
    TK_TYPESPEC,
    TK_ID,
    TK_ASSIGN,
    TK_EXPR,
    TK_SEMICOLON,
    END_SEQ
};

enum TokenType assseq[] = {
    TK_VAR,
    TK_ASSOP,
    TK_EXPR,
    END_SEQ
};

enum TokenType retstseq[] = {
    TK_RETURN,
    TK_EXPR,
    TK_SEMICOLON,
    END_SEQ
};

enum TokenType *seqtable[] = {
    funcseq,
    exprstseq,
    vardecseq,
    varinitseq,
    assseq,
    retstseq
};

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

void free_tree(struct ASTNode *root) {
    struct ASTNode *current = root->fisrt_child;
    while (current) {
        struct ASTNode *temp = current->next_sibling;
        free_tree(current);
        current = temp;
    }
    free(root);
}

void print_tree(struct ASTNode *root) {
    struct ASTNode *current = root->fisrt_child;
    if (!current) return;
    printf("[%d, %s] -> ", root->type, root->value);
    struct ASTNode *children[32];
    int num_child = 0;
    while (current) {
        printf("[%d, %s], ", current->type, current->value);
        children[num_child++] = current;
        current = current->next_sibling;
    }
    printf("\n");
    int i;
    for (i=0; i<num_child; i++)
        print_tree(children[i]);
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
    struct Token *token_ptr_backup = *token_ptr;
    if (type < END_BASICS) {
        if ((*token_ptr)->type != type) return NULL;
        struct ASTNode *root = new_node(type);
        strcpy(root->value, (*token_ptr)->value);
        (*token_ptr) = (*token_ptr)->next;
        return root;
    }
    if (type < END_SEQ) {
        struct ASTNode *root = new_node(type);
        enum TokenType *seq = seqtable[type - TK_FUNCDEF];
        int i;
        for (i=0; seq[i] != END_SEQ; i++) {
            struct ASTNode *child = match(seq[i], token_ptr);
            if (!child) {
                *token_ptr = token_ptr_backup;
                free_tree(root);
                return NULL;
            }
            if (
                isdelim(seq[i])
                || type == TK_VARINIT && seq[i] == TK_ASSIGN
                || type == TK_RETST && seq[i] == TK_RETURN
            )
                continue;
            add_child(root, child);
        }
        return root;
    }
    if (type < END_CANDS) {
        enum TokenType *cand = cand_table[type - TK_TYPESPEC];
        int i;
        for (i=0; cand[i] != END_CANDS; i++) {
            struct ASTNode *root = match(cand[i], token_ptr);
            if (root) return root;
        }
        return NULL;
    }
    if (type < END_REPCANDS) {
        struct ASTNode *root = new_node(type);
        struct ASTNode *child;
        enum TokenType *repcand = repcands_table[type - TK_PROGRAM];
        while (1) {
            int i;
            for (i=0; repcand[i] != END_REPCANDS; i++) {
                child = match(repcand[i], token_ptr);
                if (child) break;
            }
            if (!child) break;
            add_child(root, child);
        }
        return root;
    }
    return NULL;
}

struct ASTNode *parse(struct TokenStream tokens) {
    return match(TK_PROGRAM, &tokens.head);
}

