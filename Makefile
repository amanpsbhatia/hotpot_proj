log_file.o:
	gcc -g -I../ log_file.c index.c main.c txn_mn.c -o log_file.o

clean:
	rm -f *.o

