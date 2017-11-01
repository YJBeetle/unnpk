all:	unnpk mapnpk

unnpk:	unnpk.c
	gcc unnpk.c -o unnpk -lz -lmagic

mapnpk:	mapnpk.c
	gcc mapnpk.c -o mapnpk

clean:
	rm -rf unnpk
	rm -rf mapnpk