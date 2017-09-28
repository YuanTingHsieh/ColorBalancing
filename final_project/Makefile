# CC and CFLAGS are varilables
CC = g++
CFLAGS = -c -W
AR = ar
ARFLAGS = rcv
# -c option ask g++ to compile the source files, but do not link.
# -g option is for debugging version
# -O3 option is for optimized version
DBGFLAGS = -g -DEBUG
OPTFLAGS = -O3

all	: bin/DPT_balance_color
	@echo -n ""

# optimized version
bin/DPT_balance_color	 : graph_opt.o main_opt.o tm_usage.o ga.o
			$(CC) $(OPTFLAGS) graph_opt.o main_opt.o tm_usage.o ga.o -o bin/DPT_balance_color
ga.o 		: GA/ga.cpp GA/ga.h
			$(CC) $(CFLAGS) $< -Ilib -o $@
tm_usage.o 		: src/tm_usage.cpp src/tm_usage.h
			$(CC) $(CFLAGS) $< -Ilib -o $@
main_opt.o 	    : src/main.cpp
			$(CC) $(CFLAGS) $< -Ilib -o $@
graph_opt.o	    : src/graph.cpp src/graph.h 
			$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@

# DEBUG Version
dbg : bin/DPT_balance_color_dbg
	@echo -n ""

bin/DPT_balance_color_dbg	    : graph_dbg.o main_dbg.o tm_usage_dbg.o ga_dbg.o
			$(CC) $(DBGFLAGS) graph_dbg.o main_dbg.o tm_usage_dbg.o ga_dbg.o -o bin/DPT_balance_color_dbg
ga_dbg.o 		: GA/ga.cpp GA/ga.h
			$(CC) $(CFLAGS) $< -Ilib -o $@
tm_usage_dbg.o 		: src/tm_usage.cpp src/tm_usage.h
			$(CC) $(CFLAGS) $< -Ilib -o $@
main_dbg.o 	   	: src/main.cpp
			$(CC) $(CFLAGS) $< -Ilib -o $@
graph_dbg.o	    : src/graph.cpp src/graph.h 
			$(CC) $(CFLAGS) $(DBGFLAGS) $< -o $@

# clean all the .o and executable files
clean:
		rm -rf *.o bin/*

