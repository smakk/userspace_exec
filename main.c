#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp){
	//void * fd = malloc(1024*1024);

	void (*us_exec)(int, char **, char **);
	void *lib_us_exec = dlopen("./us_exec.so", RTLD_NOW | RTLD_DEEPBIND);
	if(!lib_us_exec){
		printf("[main]:open lib wrong\n");
		exit(1);
	}
	us_exec = (void (*)(int, char **, char **))dlsym(lib_us_exec, "us_exec");
	if(!us_exec){
		printf("[main]:open us_exec wrong\n");
		exit(1);
	}
	//打印pid，暂停一段时间，这样可以根据pid来查看进程的map文件，观察进程空间布局情况，检查map和munmap的执行情况
	printf("[main]:pid is %d\n", getpid());
	//sleep(60);
	us_exec(argc, argv, envp);

	/*
	printf("[main]:%d\n", argc);
	printf("[main]:%p\n", argv);
	printf("[main]:%p\n", envp);
	*/
	return 0;
}
