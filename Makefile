all:	unnpk mapnpk

unnpk:	unnpk.c
	gcc unnpk.c -o unnpk -lz -lmagic -std=gnu99

mapnpk:	mapnpk.c
	gcc mapnpk.c -o mapnpk -std=gnu99

clean:
	rm -rf unnpk
	rm -rf mapnpk
