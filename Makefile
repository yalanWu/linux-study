src=$(wildcard *.c)
obj=$(subpast %.c %.o $(src))

app:$(obj)
	gcc stat.c -o stat
