CC= gcc
CFLAGS= -g -Wall
LDFLAGS= `xml2-config --libs`
CPPFLAGS= `xml2-config --cflags`
SRC= $(wildcard *.c)
EXE= $(SRC:.c=.exe)

all: $(EXE)

%.exe: %.c
	$(CC) $(CFLAGS) $< -o $@ $(CPPFLAGS) $(LDFLAGS)

clean:
	-rm -f *.exe
