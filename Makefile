CC = g++
CFLAGS = -g -Wall -I/src
SRCDIR = src/
SRCS = $(wildcard $(SRCDIR)/*.cpp)
PROG = cap2char

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV) -lstdc++fs

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

all: mv $(PROG) ../

clean: ; rm -f *.o $(PROG)
