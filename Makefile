all:us_exec main

main:
	gcc -o main main.c
us_exec:
	gcc -shared -fPIC -o us_exec.so us_exec.c
clean:
	rm us_exec.so main *~
