CC=mpicc 
CFLAGS= -Wall
BINDIR= bin
SRCEXT= c
SOURCES := $(shell find -type f -name *.$(SRCEXT))
OBJECTS := $(SOURCES:.$(SRCEXT)=.o)
LIBS= -lsndfile

all: exec 

build: $(OBJECTS) 
		@mkdir -p $(BINDIR)
		$(CC) $(CFLAGS) $^  -o $(BINDIR)/server $(LIBS)
		@$(RM) -r $(OBJECTS)

%.o: %.$(SRCEXT)
	$(CC) $(CFLAGS) -c $< -o $@

exec: build
	mpirun -np 3 ./$(BINDIR)/server $(ARGS)


debug: build
	gdb $(BINDIR)/client