#include "lexer.h"
#include "parser.h"
#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extract_name(char path[], char out[]) {
    char *slash = strrchr(path, '/');
    char *dot = strrchr(path, '.');
    char *ptr;
    int i = 0;
    for (ptr = ++slash; ptr != dot; ptr++) {
        out[i] = *ptr;
        i++;
    }
    out[i] = '\0';
}

char *read_file(char filename[]) {
    /* Read the whole file into a string */
    FILE *fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    char *buffer = (char*)malloc(size * sizeof(char));
    fseek(fp, 0, SEEK_SET);
    fread(buffer, sizeof(char), size, fp);
    fclose(fp);
    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please give the target file name.\n");
        return 1;
    }

    char *source = read_file(argv[1]);
    struct TokenStream tokens = lex(source);
    free(source);
    print_tokens(tokens);

    struct ASTNode *ast = parse(tokens);
    free_tokens(tokens);
    print_tree(ast);

    char outname[32];
    extract_name(argv[1], outname);
    FILE *fp = fopen(outname, "w");
    gencode(ast, fp);
    fclose(fp);

    free_tree(ast);
    return 0;
}

