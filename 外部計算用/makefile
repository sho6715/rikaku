
CPPOPTIONS="-Wno-deprecated"
HEADERS=""
CFLAGS   = -g -Wall -O0

all: turn.exe

clean:
	rm *.o *.exe

turn.exe: sla_turn_calc.o
	g++ -o $@ $^

.cpp.o: ${HEADERS}
	g++ ${CPPOPTIONS} -c $<


.SUFFIXES:	.h .cpp .o .exe
