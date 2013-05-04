
CXX=g++
CXXFLAGS = -O2
#CXXFLAGS += -DDEBUG

test: test.exe
	./test.exe || echo ""

test.exe: test.o lexer.o lexer.test.o parser.o parser.test.o context.o context.test.o
	g++ -o test.exe test.o lexer.o lexer.test.o parser.o parser.test.o context.o context.test.o -lcppunit

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

lexer.o: lexer.h
lexer.test.o: lexer.h

parser.o: lexer.h parser.h node.h
parser.test.o: lexer.h parser.h node.h

context.o: context.h node.h log.h
context.test.o: lexer.h parser.h node.h context.h

clean:
	rm -f *.o *.exe
