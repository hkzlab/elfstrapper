CC=gcc

SRCDIR=src/
BINDIR=bin/

CFLAGS=-O2 -Wall -std=c99 -I$(SRCDIR)/

PROGNAME=elfstrapper

$(BINDIR)/$(PROGNAME):	$(BINDIR)/main.o 
	$(CC) $(CFLAGS)	$(BINDIR)/main.o -o $(BINDIR)/$(PROGNAME) 

$(BINDIR)/main.o:	$(SRCDIR)/main.c
	$(CC) $(CFLAGS)	$(SRCDIR)/main.c -c -o $(BINDIR)/main.o

clean:
	rm -rf $(BINDIR)/$(PROGNAME)
	rm -rf $(BINDIR)/*.o

