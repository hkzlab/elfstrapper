CC=gcc

SRCDIR=src/
BINDIR=bin/

CFLAGS=-O2 -Wall -std=c99 -I$(SRCDIR)/

PROGNAME=elfstrapper

$(BINDIR)/$(PROGNAME):	$(BINDIR)/main.o $(BINDIR)/parport.o $(BINDIR)/elf.o
	$(CC) $(CFLAGS)	$(BINDIR)/main.o $(BINDIR)/parport.o $(BINDIR)/elf.o -o $(BINDIR)/$(PROGNAME) 

$(BINDIR)/main.o:	$(SRCDIR)/main.c
	$(CC) $(CFLAGS)	$(SRCDIR)/main.c -c -o $(BINDIR)/main.o

$(BINDIR)/parport.o:	$(SRCDIR)/parport/parport.c
	$(CC) $(CFLAGS)	$(SRCDIR)/parport/parport.c -c -o $(BINDIR)/parport.o

$(BINDIR)/elf.o:	$(SRCDIR)/elf/elf.c
	$(CC) $(CFLAGS)	$(SRCDIR)/elf/elf.c -c -o $(BINDIR)/elf.o

clean:
	rm -rf $(BINDIR)/$(PROGNAME)
	rm -rf $(BINDIR)/*.o

