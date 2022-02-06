CC = g++
CFLAGS = -o
LANG = -std=c++14
YACC = bison
LEX = flex
YFLAGS = -d
BIN = parser
YFILE = parser.yy
LFILE = parser.l
YOUT = parser.cc parser.hh
LOUT = lex.yy.c
DEP = execute.cpp

OBJ = main.o
FLAGS = -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
SFML = sfml-app
SOURCE = main.cpp

all : $(BIN) $(SFML)

$(BIN) : $(LOUT) $(YOUT) $(DEP)
	$(CC) $(LANG) $^ $(CFLAGS) $@
$(LOUT) : $(LFILE)
	$(LEX) $^
$(YOUT) : $(YFILE)
	$(YACC) $(YFLAGS) $^ $(CFLAGS) $@
	
$(SFML): $(OBJ)
	$(CC) $(OBJ) $(FLAGS)
$(OBJ): $(SOURCE)
	$(CC) -c $(SOURCE)
clean:
	rm -rf *.o $(BIN)
	rm -rf *.o $(YOUT)
	rm -rf *.o $(LOUT)
	rm -rf *.o $(SFML)
	rm -rf *.o $(OBJ)
