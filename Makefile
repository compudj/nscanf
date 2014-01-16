all: libnscanf.so nscanf-test

nscanf.o: nscanf.c nscanf.h
	gcc -Wall -g -c -fPIC -o $@ $<

libnscanf.so: nscanf.o
	gcc -Wall -g -shared -o $@ $<

nscanf-test: libnscanf.so nscanf-test.c nscanf.h
	gcc -Wall -g -L./ -lnscanf -o $@ nscanf-test.c

.PHONY: clean

clean:
	rm -f nscanf-test libnscanf.so *.o
