long my_write(long fd, char *buf, unsigned long len)
{
	long ret;
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov %1, %%rsi\n"
		"mov %2, %%rdx\n"
		"mov $1, %%rax\n"
		"syscall" : : "g"(fd), "g"(buf), "g"(len));
	asm("mov %%rax, %0" : "=r"(ret));
	return ret;
}

char * my_itoa(long x, char *t)
{
	int i;
	int j;
	i = 0;
	do
	{
		t[i] = (x % 10) + '0';
		x /= 10;
		i++;
	} while (x!=0);
	t[i] = 0;
	for (j=0; j < i / 2; j++) {
		t[j] ^= t[i - j - 1];
		t[i - j - 1] ^= t[j];
		t[j] ^= t[i - j - 1];
	}
	return t;
}

void us_exec(int argc, char **argv, char **env){
	char tmp[10];
	my_write(1,my_itoa(argc,tmp),1);
	my_write(1,argv[0],10);
	my_write(1,env[0],10);
}

