CC=g++
CPPFLAGS=-Wall

all: Brill-train Brill-test

clean:
	rm *.o

Brill-train: Brill-train.o
	$(CC) $(CPPFLAGS) -o Brill-train Brill-train.o

Brill-test: Brill-test.o
	$(CC) $(CPPFLAGS) -o Brill-test Brill-test.o

Brill-train.o: Brill-test.cpp
	$(CC) $(CPPFLAGS) -c -fPIC Brill-train.cpp

Brill-test.o: Brill-test.cpp
	$(CC) $(CPPFLAGS) -c -fPIC Brill-test.cpp

corpus.o: src/corpus.h src/corpus.cpp
	$(CC) $(CPPFLAGS) -fPIC -c src/corpus.cpp

rules.o: src/rules.h src/rules.cpp
	$(CC) $(CPPFLAGS) -fPIC -c src/rules.cpp

rules_applier.o: src/rules_applier.h src/rules_applier.cpp
	$(CC) $(CPPFLAGS) -fPIC -c src/rules_applier.cpp

rules_scoretab.o: src/rules_scoretab.h src/rules_scoretab.cpp
	$(CC) $(CPPFLAGS) -fPIC -c src/rules_scoretab.cpp
