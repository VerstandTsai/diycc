INC = ./includes
SRC = ./src

CC = gcc
CFLAGS = -std=c89 -I$(INC)
RM = rm

all: diycc

diycc: lexer.o parser.o
	$(CC) $(CFLAGS) -o $@ $(SRC)/diycc.c $^

%.o: $(SRC)/%.c $(INC)/%.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) -f *.o

