CC=g++
DEFS=  -D_REENTRANT -D_GNU_SOURCE
INC= -I./ -I../
#OPT= -pipe -O3 -g
OPT= -O1 -g
WARN_OPTS= -W -Wall -Wpointer-arith

CFLAGS= -g -m64 $(WARN_OPTS) $(OPT) $(INC) $(DEFS)
#LFLAGS= -g -m64 -lpthread
LFLAGS= -g -m64 -pthread

SRC= BaseAddr.cpp \
InetAddr.cpp \
UnixAddr.cpp \
Sock.cpp \
SockAcceptor.cpp \
SockConnector.cpp \
SockDgram.cpp \
SockStream.cpp \
Poll.cpp \
LapTimer.cpp \
SharedMem.cpp \
ThreadSvc.cpp \
LogMsg.cpp

OBJ=$(SRC:.cpp=.o)

MYLIB = libOsPort.a

all: $(MYLIB) main

main: $(MYLIB) main.cpp
	$(CC) $(LFLAGS) -o osport_test.exe main.cpp $(MYLIB)

$(MYLIB): $(OBJ)
	ar rcv $@ $(OBJ)

see:
	ar t $(MYLIB)

.SUFFIXES: .pc .c .cpp .o .typ .h

.cpp.o: $(SRC)
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o


clean:
	/bin/rm -f *.o $(EXE) $(MYLIB)


