all:
	gcc -c -o cursor.o cursor.c
	ar rcs libcursor.a cursor.o
