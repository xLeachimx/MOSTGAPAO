COMPILEDFILES = Genetics.o Object.o
TEMPFOLDER = TEMP
PROGNAME = STGA

all: main

main: copy $(COMPILEDFILES)
	g++ $(COMPILEDFILES) main.cpp -o $(PROGNAME)

copy:
	mkdir $(TEMPFOLDER)
	cp Genetics/* $(TEMPFOLDER)/
	cp Object/* $(TEMPFOLDER)/

Genetics.o:
	g++ $(TEMPFOLDER)/Genetics.cpp -c

Object.o:
	g++ $(TEMPFOLDER)/Object.cpp -c

clean:
	rm -rf $(TEMPFOLDER)
	rm *.o