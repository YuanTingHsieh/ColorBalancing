CC= g++
OPTFLAGS = -O3
DBGFLAGS = -g

all	: test
	@echo -n ""

test : ga.cpp ga.h main.cpp
	$(CC) $(OPTFLAGS) main.cpp ga.cpp ga.h -o test