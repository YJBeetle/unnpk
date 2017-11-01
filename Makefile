all:
	gcc main.c -o unnpk -lz -lmagic

clean:
	rm unnpk