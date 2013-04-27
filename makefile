Pscript:lex.o main.o parse.o
	g++ lex.o parse.o synttree.o symtab.o main.o -o Pscript
lex.o:
	flex -olex.cpp token.l
	g++ -c lex.cpp -o lex.o
parse.o:
	g++ -c  parse.cpp -o parse.o
synttree.o:
	g++ -c synttree.cpp -o synttree.o
symtab.o:
	g++ -c symtab.cpp -o symtab.o
main.o:
	g++ -c main.cpp -o main.o
clean:
	rm -rf *.o Pscript
	rm -f lex.cpp
	rm -f parse.cpp parse.output parse.hpp
