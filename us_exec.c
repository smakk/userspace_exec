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

long my_open(const char *path, unsigned long flags, long mode)
{
	long ret;
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov %1, %%rsi\n"
		"mov %2, %%rdx\n"
		"mov $2, %%rax\n"
		"syscall" : : "g"(path), "g"(flags), "g"(mode));
	asm ("mov %%rax, %0" : "=r"(ret));
	return ret;
}

int my_read(long fd, char *buf, unsigned long len)
{
	long ret;
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov %1, %%rsi\n"
		"mov %2, %%rdx\n"
		"mov $0, %%rax\n"
		"syscall" : : "g"(fd), "g"(buf), "g"(len));
	asm("mov %%rax, %0" : "=r"(ret));
	return (int)ret;
}

int my_close(unsigned int fd)
{
	long ret;
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov $3, %%rax\n"
		"syscall" : : "g"(fd));
	return (int)ret;
}

void my_exit(long status)
{
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov $60, %%rax\n"
		"syscall" : : "r"(status));
}

char *strchr(const char *s, int c)
{
	char *r = 0;
	for (;s&&*s;++s)
	{
		if(*s==c)
		{
			r=(char *)s;
			break;
		}
	}
	return r;
}

char * itoa(long x, char *t)
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

unsigned long strlen(const char *s)
{
	unsigned long r = 0;
	for (; s && *s; ++s, ++r);
	return r;
}

char *strstr(const char *str, char *substr)
{
	char *r = 0;
	int substrl = strlen(substr);
	int strl = strlen(str);

	if (substrl<strl)
	{
		int i;
		for (i=0;i<=strl-substrl;++i)
		{
			char *p = (char *)&str[i];
			int j;

			for (j=0;j<substrl;++j)
			{
				if (p[j]!=substr[j])
					break;
			}

			if (j==substrl)
			{
				r = p;
				break;
			}
		}
	} else if (substrl == strl) {
		int i;
		char *p = (char *)&str[0];
		for (i=0;i<substrl;++i)
		{
			if (p[i]!=substr[i])
				break;
		}
		if (i==substrl)
			r = p;
	}

	return r;
}

unsigned long int strtoul(char *nptr, char **endptr, int base)
{
	unsigned long ret = 0;
	int i;

	for (i=0; nptr[i]; ++i)
	{
		char digit = nptr[i];
		unsigned int value;
		if (digit<='9')
		{
			value='0';
		} else if (digit<='Z') {
			value='A'-10;
		} else if (digit<='z') {
			value='a'-10;
		}
		ret *= base;
		ret += (digit - value);
		if (endptr) 
			*endptr = &(nptr[i]);
	}
	return ret;
}


void us_exec(int argc, char **argv, char **env){
	char buf[1024], *p;
	char rbuf[2048];
	int cc, fd;

	fd = my_open("/proc/self/maps", 0, 0);
	
	p = &buf[0];
	while (0 < (cc = my_read(fd, rbuf, sizeof(rbuf)))){
		int i;
		for (i = 0; i < cc; ++i)
		{
			int c = rbuf[i];
			if ('\n' != c)
				*p++ = c;
			else {
				*p = '\0';
				if(!strstr(buf, "libdl")){
					
					char *u;
					char *first, *second;
					unsigned long low, high;
					u = strchr(buf, ' ');
					*u = '\0';
					first = buf;
					second = strchr(first, '-');
					*second = '\0';
					++second;
					low = strtoul(first, 0, 0x10);
					high = strtoul(second, 0, 0x10);

					write(1,buf,p-buf);
					char ff[1] = {'\n'};
					write(1,ff,1);

					
				}
				p = &buf[0];
			}
		}
	}
	my_close(fd);

	/*
	char tmp[10];
	my_write(1,my_itoa(argc,tmp),1);
	my_write(1,argv[0],10);
	my_write(1,env[0],10);
	*/
}
