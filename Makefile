
CC=g++
CFLAGS=-I.

CPPFLAGS=$(CFLAGS)

EXE=scriptr

OBJ=	common.o parson.o CScript.o main.o

all: $(EXE)

%.o: %.c %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o $(EXE)
