CC=g++

CFLAGS=-c 
IFLAGS=-I../../

SOURCES=main.cpp IR-parser.cpp translator.cpp input_output.cpp
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=../../trollator

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(IFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(IFLAGS) $(CFLAGS) $< -o $@