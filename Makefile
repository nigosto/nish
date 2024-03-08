sources := ./src/utils/utils.c ./src/lexer/lexer.c ./src/parser/parser.c ./src/pipe/pipe.c ./src/redirection/redirection.c ./src/main.c

run:
	clang $(sources) -o main.exe -fsanitize=address && ./main.exe

clean:
	rm -f *.exe