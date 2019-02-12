parser: Tables.o Executor.o Expression.o MyItr.o Btree.o Tuple.o main.o QuadraticProbing.o Global.o lex.yy.cc sql.ypp.tab.c
	g++ -g -std=c++11 -o parser Global.o Executor.o Tables.o Expression.o MyItr.o Rewriter.o Btree.o Tuple.o QuadraticProbing.o main.o 
 
main.o: parser.cpp Expression.o Rewriter.o
	g++ -g -std=c++11 -c -o main.o parser.cpp  

Expression.o: Expression.cpp Expression.h sql.ypp.tab.c lex.yy.cc	
	g++ -g -std=c++11 -c -o Expression.o Expression.cpp 

Tables.o: Tables.h Tables.cpp Global.h MyItr.h MyItr.cpp Btree.h Btree.cpp Tuple.h Tuple.cpp Global.h Global.cpp
	g++ -g -std=c++11 -c -o Tables.o Tables.cpp

Executor.o: Executor.cpp Executor.h Global.h Global.h Global.cpp
	g++ -g -std=c++11 -c -o Executor.o Executor.cpp

MyItr.o: MyItr.cpp MyItr.h Global.h Global.cpp
	g++ -g -std=c++11 -c -o MyItr.o MyItr.cpp

Rewriter.o: Rewriter.cpp Rewriter.h
	g++ -g -std=c++11 -c -o Rewriter.o Rewriter.cpp

QuadraticProbing.o: QuadraticProbing.cpp QuadraticProbing.h
	g++ -g -std=c++11 -c -o QuadraticProbing.o QuadraticProbing.cpp

Global.o: Global.h Global.cpp
	g++ -g -std=c++11 -c -o Global.o Global.cpp

sql.ypp.tab.c: sql.ypp
	bison sql.ypp

lex.yy.cc: sql.x
	flex -c++ sql.x


# for B tree

Btree.o: Btree.cpp Btree.h Tuple.o
	g++ -Wall -std=c++11 -ansi -g -c Btree.cpp

Tuple.o: Tuple.h Tuple.cpp
	g++ -Wall -std=c++11 -ansi -g -c Tuple.cpp


clean:
	rm parser *.o



# bison sql.ypp

# flex --c++ sql.x

# g++ testparser.c