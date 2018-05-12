CC = g++
FLAGS=-std=c++11 -Wall -g

library.o: library.cc library.h
	$(CC) $(FLAGS) -o library.o -c library.cc

experiment_1_1: experiment_1_1.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

write_fixed_len_pages: write_fixed_len_pages.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

read_fixed_len_pages: read_fixed_len_pages.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

csv2heapfile: csv2heapfile.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

scan: scan.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

insert: insert.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

update: update.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

delete: delete.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

select: select.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

csv2colstore: csv2colstore.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

select2: select2.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

select3: select3.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

all: select3 select2 csv2colstore select delete update insert scan csv2heapfile read_fixed_len_pages write_fixed_len_pages experiment_1_1

clean:
	-rm -f *.o .*swp .*swo
