Pscript:lex.o main.o parse.o
	g++ lex.o parse.o main.o -o Pscript
lex.o:
	flex -olex.cpp token.l
	g++ -c lex.cpp -o lex.o
parse.o:
	g++ -c  parse.cpp -o parse.o
main.o:
	g++ -c main.cpp -o main.o
clean:
	rm -rf *.o Pscript
	rm -f lex.cpp
	rm -f parse.cpp parse.output parse.hpp
