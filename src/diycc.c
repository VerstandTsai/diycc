#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>

char *read_file(char filename[]) {
    // Read the whole file into a string
    FILE *fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    char *buffer = (char*)malloc(size * sizeof(char));
    fseek(fp, 0, SEEK_SET);
    fread(buffer, sizeof(char), size, fp);
    fclose(fp);
    return buffer;
}

void compile(char code[]) {
    struct TokenStream tokens = lex(code);
    print_tokens(tokens);
    free_tokens(tokens);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please give the target file name.\n");
        return 1;
    }
    char *code = read_file(argv[1]);
    compile(code);
    free(code);
    return 0;
}

