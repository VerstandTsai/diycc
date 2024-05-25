#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct Token *new_token(enum TokenType type, char value[]) {
    struct Token *token = (struct Token*)malloc(sizeof(struct Token));
    strcpy(token->value, value);
    token->type = type;
    token->next = NULL;
    return token;
}

struct TokenStream tokenstream() {
    struct TokenStream tokens;
    tokens.head = NULL;
    tokens.tail = NULL;
    return tokens;
}

void append_token(struct TokenStream *tokens, struct Token *token) {
    if (!tokens->head) {
        tokens->head = token;
        tokens->tail = token;
        return;
    }
    tokens->tail->next = token;
    tokens->tail = tokens->tail->next;
}

void free_tokens(struct TokenStream tokens) {
    struct Token *current = tokens.head;
    while (current) {
        struct Token *next = current->next;
        free(current);
        current = next;
    }
}

void print_tokens(struct TokenStream tokens) {
    struct Token *current = tokens.head;
    while (current) {
        printf("[%d, %s]\n", current->type, current->value);
        current = current->next;
    }
}

void lexer_error(enum LexerError error) {
    switch (error) {
    case LEXERR_TOOLONG:
        printf("Lexer error: token too long\n");
    case LEXERR_UNKNOWN:
        printf("Lexer error: unknown symbol\n");
    }
    exit(1);
}

struct TokenStream lex(char code[]) {
    char *keywords[] = {
        "int", "return"
    };
    char *symbols[] = {
        "=", ";", "(", ")", "{", "}"
    };
    struct TokenStream tokens = tokenstream();
    char *pos = code;
    while (1) {
        char c = *pos;
        if (!c) break;
        if (isspace(c)) {
            pos++;
            continue;
        }
        if (isalpha(c)) {
            /* We are reading either a keyword or an identifier */
            char value[MAX_ID_LENGTH];
            int index = 0;
            while (isalnum(*pos)) {
                if (index > MAX_ID_LENGTH - 2) lexer_error(LEXERR_TOOLONG);
                value[index++] = *pos++;
            }
            value[index] = '\0';
            int iskeyword = 0;
            int i;
            for (i=0; i<END_KEYWORDS-END_SYMBOLS-1; i++) {
                if (strcmp(value, keywords[i]) == 0) {
                    append_token(&tokens, new_token(END_SYMBOLS+1+i, value));
                    iskeyword = 1;
                    break;
                }
            }
            if (!iskeyword) append_token(&tokens, new_token(TK_ID, value));
        } else if (isdigit(c)) {
            /* We are reading a number literal */
            char value[MAX_ID_LENGTH];
            int index = 0;
            while (isdigit(*pos)) {
                if (index > MAX_ID_LENGTH - 2) lexer_error(LEXERR_TOOLONG);
                value[index++] = *pos++;
            }
            value[index] = '\0';
            append_token(&tokens, new_token(TK_NUMBER, value));
        } else {
            /*
             * We are reading symbols
             * This is going to be different than the previous two types
             * because we cannot reliably determine the end of a symbol token,
             * so we will instead compare each possible tokens
             * with our code at the current position.
             */
            int isunknown = 1;
            int i;
            for (i=0; i<END_SYMBOLS; i++) {
                int symbol_length = strlen(symbols[i]);
                if (strncmp(pos, symbols[i], symbol_length) == 0) {
                    append_token(&tokens, new_token(i, symbols[i]));
                    pos += symbol_length;
                    isunknown = 0;
                    break;
                }
            }
            if (isunknown) lexer_error(LEXERR_UNKNOWN);
        }
    }
    return tokens;
}

