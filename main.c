#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char **envp){
	printf("%d\n", argc);
	printf("%p\n", argv);
	printf("%p\n", envp);
}
