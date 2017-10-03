
CC=g++
CFLAGS= -g -I.

CPPFLAGS=$(CFLAGS)

EXE=scriptr
CGIEXE=scriptr.cgi

OBJ=	common.o parson.o CScript.o main.o
CGIOBJ=	common.o parson.o CScript.o main_cgi.o

all: $(EXE) $(CGIEXE)

%.o: %.c %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(CGIEXE): $(CGIOBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lcgicc

clean:
	rm -f *.o $(EXE) $(CGIEXE)
