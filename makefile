MPICC=mpicc 
CC=gcc
CFLAGS= -Wall
BINDIR= bin
SRCEXT= c
SOURCES := $(shell find -type f -name *.$(SRCEXT))
OBJECTS := $(SOURCES:.$(SRCEXT)=.o)
LIBS= -lsndfile


dot_product_mpi: 
	@mkdir -p $(BINDIR)
	$(MPICC) $(CFLAGS) -o $(BINDIR)/dot_product_mpi dot_product_mpi.c
	@$(RM) -r dot_product_mpi.o	
	mpirun -np 3 --hostfile hosts ./$(BINDIR)/dot_product_mpi $(ARGS)

dot_product: 
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/dot_product dot_product_seq.c
	@$(RM) -r dot_product.o	
	./$(BINDIR)/dot_product $(ARGS)

run_seq: dot_product
	@mkdir -p output
	@for i in $$(seq 1 10); do \
		./$(BINDIR)/dot_product $(ARGS) > output/output_seq_$${i}.txt; \
	done


