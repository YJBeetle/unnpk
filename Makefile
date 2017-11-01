all:
	gcc unnpk.c -o unnpk -lz -lmagic

clean:
	rm unnpk