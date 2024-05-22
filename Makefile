INC = ./includes
SRC = ./src

CC = gcc
CFLAGS = -I$(INC)
RM = rm

all: diycc

diycc: lexer.o
	$(CC) $(CFLAGS) -o $@ $(SRC)/diycc.c $^

%.o: $(SRC)/%.c $(INC)/%.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) -f *.o

