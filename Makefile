all:
	g++ main.cpp -o main.exe

run:
	./main.exe

pack:
	zip hw.zip *.cpp Makefile
# expression_parser.cpp expression.cpp binary_operation.cpp unary_operation.cpp variable.cpp 
