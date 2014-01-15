all: libnscanf.so nscanf-test

nscanf.o: nscanf.c nscanf.h
	gcc -c -fPIC -o $@ $<

libnscanf.so: nscanf.o
	gcc -shared -o $@ $<

nscanf-test: libnscanf.so nscanf-test.c nscanf.h
	gcc -L./ -lnscanf -o $@ nscanf-test.c

.PHONY: clean

clean:
	rm -f nscanf-test libnscanf.so *.o
