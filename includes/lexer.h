#ifndef LEXER_H
#define LEXER_H

#define MAX_ID_LENGTH 32

enum TokenType {
    TK_ASSIGN, // '='
    TK_SEMICOLON, // ';'
    TK_LPAREN, // '('
    TK_RPAREN, // ')'
    TK_LBRACE, // '{'
    TK_RBRACE, // '}'
    END_SYMBOLS,
    TK_INT,
    TK_RETURN,
    END_KEYWORDS,
    TK_ID,
    TK_NUMBER,
    TK_PROGRAM,
    TK_FUNCDEF,
    TK_TYPESPEC,
    TK_BODY,
    TK_STATEMENT,
    TK_VARDEC, // 'int a;'
    TK_VARINIT, // 'int a = 0;'
    TK_VAR,
    TK_ASSIGNMENT, // 'a = b'
    TK_ASSOP, // '=', '+=', '*=' etc.
    TK_EXP, // Anything that returns a value
    TK_RETST // 'return a;'
};

struct Token {
    enum TokenType type;
    char value[MAX_ID_LENGTH];
    struct Token *next;
};

struct TokenStream {
    struct Token *head;
    struct Token *tail;
};

struct Token *new_token(enum TokenType type, char value[]);
struct TokenStream tokenstream();
void append_token(struct TokenStream *tokens, struct Token *token);
void free_tokens(struct TokenStream tokens);
void print_tokens(struct TokenStream tokens);

enum LexerError {
    LEXERR_TOOLONG,
    LEXERR_UNKNOWN
};

void lexer_error(enum LexerError error);
struct TokenStream lex(char code[]);

#endif // LEXER_H

