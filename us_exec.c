#include <fcntl.h>
#include <elf.h>
#include <sys/mman.h>

#define ROUNDUP(x, y)   ((((x)+((y)-1))/(y))*(y))
#define ALIGN(k, v) (((k)+((v)-1))&(~((v)-1)))
#define ALIGNDOWN(k, v) ((unsigned long)(k)&(~((unsigned long)(v)-1)))

char * itoa(long x, char *t);
unsigned long strlen(const char *s);

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

void *my_mmap(void *addr, unsigned long len, unsigned long prot, unsigned long flags, long fd, unsigned long off)
{
	/*
	char tmp[20];
	my_write(1, itoa((unsigned long)addr,tmp), strlen(tmp));
	my_write(1,"\n",1);
	*/	
	long mmap_fd = fd;
	unsigned long mmap_off = off;
	unsigned long mmap_flags = flags;
	unsigned long ret;
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov %1, %%rsi\n"
		"mov %2, %%rdx\n"
		"mov %3, %%r10\n"
		"mov %4, %%r8\n"
		"mov %5, %%r9\n"
		"mov $9, %%rax\n"
		"syscall\n" : : "g"(addr), "g"(len), "g"(prot), "g"(flags), "g"(mmap_fd), "g"(mmap_off));
	asm ("mov %%rax, %0" : "=r"(ret));              
	return (void *)ret;
}

int my_munmap(void *start, unsigned long length)
{
	long ret;
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov %1, %%rsi\n"
		"mov $11, %%rax\n"
		"syscall" : : "g"(start), "g"(length));
	asm("mov %%rax, %0" : "=r"(ret));
	return (int)ret;
}

void my_pause()
{
	__asm__ volatile(
		"mov $34, %%rax\n"
		"syscall" : :);
}

int my_mprotect(void *addr, unsigned long len, int prot)
{
	long ret;
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov %1, %%rsi\n"
		"mov %2, %%rdx\n"
		"mov $10, %%rax\n"
		"syscall" : : "g"(addr), "g"(len), "g"(prot));
	asm("mov %%rax, %0" : "=r"(ret));
	return ret;
}

void my_brk(unsigned long addr)
{
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov $12, %%rax\n"
		"syscall" : : "g"(addr));
}

int my_stat(long fd, void *buf)
{
	long ret;
	__asm__ volatile(
		"mov %0, %%rdi\n"
		"mov %1, %%rsi\n"
		"mov $5, %%rax\n"
		"syscall" : : "g"(fd), "g"(buf));
	asm("mov %%rax, %0" : "=r"(ret));
	return (int)ret;
}

char *strchr(const char *s, int c)
{
	char *r = 0;
	for (;s&&*s;++s){
		if(*s==c){
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

	if (substrl<strl){
		int i;
		for (i=0;i<=strl-substrl;++i){
			char *p = (char *)&str[i];
			int j;

			for (j=0;j<substrl;++j){
				if (p[j]!=substr[j])
					break;
			}

			if (j==substrl){
				r = p;
				break;
			}
		}
	} else if (substrl == strl) {
		int i;
		char *p = (char *)&str[0];
		for (i=0;i<substrl;++i){
			if (p[i]!=substr[i])
				break;
		}
		if (i==substrl) r = p;
	}

	return r;
}

unsigned long int strtoul(char *nptr, char **endptr, int base)
{
	unsigned long ret = 0;
	int i;

	for (i=0; nptr[i]; ++i){
		char digit = nptr[i];
		unsigned int value;
		if (digit<='9'){
			value='0';
		} else if (digit<='Z') {
			value='A'-10;
		} else if (digit<='z') {
			value='a'-10;
		}
		ret *= base;
		ret += (digit - value);
		if (endptr) *endptr = &(nptr[i]);
	}
	return ret;
}

char *strrchr(char *s, int c)
{
	char *r = 0;
	char *p;
	p = &s[strlen(s)-1];
	for (;p!=s;--p){
		if (*p==c){
			r = (char *)p;
			break;
		}
	}
	return r;
}

void *memcpy(void *dest, const void *src, unsigned long n)
{
    unsigned long i;
    unsigned char *d = (unsigned char *)dest;
    unsigned char *s = (unsigned char *)src;
    for (i = 0; i < n; ++i)
        d[i] = s[i];
    return dest;
}

int to_hex(unsigned long n, char *p){
	int i;
	int count = 0;
	for (i = 0; i < 16; ++i){
		char x = ((n >> 60)  & 0xf);
		if (x < (char)10)
			*p++ = x + '0';
		else
			*p++ = (x - 10) + 'a';
		++count;
		n <<= 4;
	}
	*p = '\0';
	return count;
}

int to_decimal(unsigned long x, char *p){
	int count = 0;
	if (x == 0)
		*p++ ='0';
	else {
		unsigned long q, r, b;
		int f = 0;
		b = 10000000000000000000U;
		do {
			q = x/b;
			if (q || f){
				*p++ = ('0' + q);
				++count;
				f = 1;
				x = x%b;
			}
			b /= 10;
		} while (b > 0);
	}
	*p = '\0';
	return count;
}

struct saved_block {
	int size;
	int cnt;
	char *block;
};

#define ALLOCATE(size)  \
	my_mmap(0, (size), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)

struct saved_block *save_argv(int argc, char **argv)
{
	struct saved_block *r = 0;
	int i, len;
	char *str;
	//len是所有字符串的长度
	if (argc > 0){
		for (i = 0, len = 0; i < argc; ++i){
			len += strlen(argv[i]) + 1;
		}
	}else {
		argc = 0;
		char **p = argv;
		while (*p){
			len += strlen(*p) + 1;
			++p;
			++argc;
		}
	}
	r = ALLOCATE(sizeof(*r));
	r->size = len;
	r->cnt = argc;
	r->block = ALLOCATE(len);

	for (i = 0, str = r->block; i < argc; i++){
		int j;
		for (j = 0; argv[i][j]; ++j)
			str[j] = argv[i][j];
		str[j] = '\0';
		str += (j + 1);
	}

	return r;
}

struct saved_block * save_elfauxv(char **envp)
{
	struct saved_block *r;
	unsigned long *p;
	int cnt;
	Elf64_auxv_t *q;
	p = (unsigned long *)envp;
	while (*p != 0)
		++p;
	++p;
	for (cnt = 0, q = (Elf64_auxv_t *)p; q->a_type != AT_NULL; ++q)
		++cnt;
	++cnt;
	r = ALLOCATE(sizeof(*r));
	r->size = sizeof(*q) * cnt;
	r->cnt = cnt;
	r->block = ALLOCATE(r->size);
	memcpy((void *)r->block, (void *)p, r->size);
	return r;
}

void release_args(struct saved_block *args)
{
	my_munmap((void *)args->block, args->size);
	my_munmap((void *)args, sizeof(*args));
}

void *stack_setup(struct saved_block *args,struct saved_block *envp,struct saved_block *auxvp,Elf64_Ehdr *ehdr,Elf64_Ehdr *ldso)
{
	Elf64_auxv_t	*aux, *excfn = 0;
	char **av, **ev;
	char	*addr, *str, *rsp;
	unsigned long *ptr;
	int	  i, j;
	char newstack[16*1024];

	rsp = (char *)(unsigned long)ALIGN(((unsigned long)(&newstack[150])), 16);

	ptr = (unsigned long *)rsp;

	*ptr++ = args->cnt;
	av = (char **)ptr;
	ptr += args->cnt;
	*ptr++ = 0;

	ev = (char **)ptr;
	ptr += envp->cnt;
	*ptr++ = 0;

	aux = (Elf64_auxv_t *)ptr;

	ptr = (unsigned long *)ROUNDUP((unsigned long)ptr + auxvp->size, sizeof(unsigned long));
	
	addr =  (char *)aux;
	for (j = 0; j < auxvp->size; ++j)
		addr[j] = auxvp->block[j];

	//重新设置一些辅助向量
	for (i = 0; i < auxvp->cnt; ++i)
	{
		switch (aux[i].a_type)
		{
		case AT_PHDR:  aux[i].a_un.a_val = (unsigned long)((char *)ehdr + ehdr->e_phoff); break;
		case AT_PHNUM: aux[i].a_un.a_val = ehdr->e_phnum; break;
		case AT_BASE:  aux[i].a_un.a_val = (unsigned long)ldso; break;
		case AT_ENTRY: aux[i].a_un.a_val = (unsigned long)ehdr->e_entry; break;
		}
	}

	*ptr++ = 0;

	addr =  (char *)ptr;
	str = args->block;

	for (i = 0; i < args->cnt; ++i)
	{
		av[i] = addr;
		for (j = 0; *str; ++j)
			*addr++ = *str++;
		*addr++ = *str++;
	}

	ptr = (unsigned long *)ROUNDUP((unsigned long)addr, sizeof(unsigned long));
	*ptr = 0;

	addr =  (char *)ptr;
	str = envp->block;

	for (i = 0; i < envp->cnt; ++i)
	{
		ev[i] = addr;
		for (j = 0; *str; ++j)
			*addr++ = *str++;
		*addr++ = *str++;
	}

	ptr = (unsigned long *)ROUNDUP((unsigned long)addr, sizeof(unsigned long));
	*ptr = 0;

	if (excfn)
	{
		addr =  (char *)ptr;
		str = args->block;
		excfn->a_un.a_val = (unsigned long)addr;
		for (j = 0; *str; ++j)
			*addr++ = *str++;
		*addr++ = *str++;

		ptr = (unsigned long *)ROUNDUP((unsigned long)addr, sizeof(unsigned long));
	}

	release_args(args);
	release_args(envp);
	release_args(auxvp);

	return ((void *)rsp);
}


void *load_elf(char *mapped, int anywhere, Elf64_Ehdr **elf_ehdr, Elf64_Ehdr **ldso_ehdr)
{
	Elf64_Ehdr *hdr;
	Elf64_Phdr *pdr, *interp = 0;
	int i;
	void *text_segment = 0;
	void *entry_point = 0;
	unsigned long initial_vaddr = 0;
	unsigned long brk_addr = 0;
	char buf[128];
	unsigned int mapflags = MAP_PRIVATE|MAP_ANONYMOUS;
	/*
	if(anywhere == 0){
		my_write(1,"qqq\n",4);
	}else{
		my_write(1,"www\n",4);
	}
	*/
	if (!anywhere){
		mapflags |= MAP_FIXED;
		//my_write(1,"wqw\n",4);
	}
		

	hdr = (Elf64_Ehdr *)mapped;
	pdr = (Elf64_Phdr *)(mapped + hdr->e_phoff);

	entry_point = (void *)hdr->e_entry;
	
	for (i = 0; i < hdr->e_phnum; ++i, ++pdr){
		unsigned int protflags = 0;
		unsigned long map_addr = 0, rounded_len, k;
		unsigned long unaligned_map_addr = 0;
		void *segment;

		if (pdr->p_type == 0x03){  // PT_INTERP
			interp = pdr;
			continue;
		}

		if (pdr->p_type != PT_LOAD)
			continue;

		if (text_segment != 0 && anywhere){
			unaligned_map_addr = (unsigned long)text_segment + ((unsigned long)pdr->p_vaddr - (unsigned long)initial_vaddr);
			map_addr = ALIGNDOWN((unsigned long)unaligned_map_addr, 0x1000);
			mapflags |= MAP_FIXED;
			//my_write(1,"111",3);
		} else if (!anywhere) {
			map_addr = ALIGNDOWN(pdr->p_vaddr, 0x1000);
			//my_write(1,"222",3);
		} else {
			map_addr = 0UL;
			//my_write(1,"333",3);
		}

		if (!anywhere && initial_vaddr == 0)
			initial_vaddr = pdr->p_vaddr;

		rounded_len = (unsigned long)pdr->p_memsz + ((unsigned long)pdr->p_vaddr % 0x1000);
		rounded_len = ROUNDUP(rounded_len, 0x1000);

		segment = my_mmap((void *)map_addr,rounded_len,PROT_WRITE|PROT_READ|PROT_EXEC, mapflags, -1, 0);
		/*
		char tmp[20];
		my_write(1, itoa((unsigned long)map_addr,tmp), strlen(tmp));
		my_write(1,"\n",1);
		*/
		if (segment == (void *) -1){
			my_write(1, "map wrong\n", 10);
		}

		memcpy(!anywhere? (void *)pdr->p_vaddr:(void *)((unsigned long)segment + ((unsigned long)pdr->p_vaddr % 0x1000)),mapped + pdr->p_offset,pdr->p_filesz);

		if (!text_segment){
			*elf_ehdr = segment;
			text_segment = segment;
			initial_vaddr = pdr->p_vaddr;
			if (anywhere)
				entry_point = (void *)((unsigned long)entry_point- (unsigned long)pdr->p_vaddr+ (unsigned long)text_segment);
		}

		if (pdr->p_flags & PF_R){
			protflags |= PROT_READ;
			//my_write(1,"www\n",4);
		}
		if (pdr->p_flags & PF_W){
			protflags |= PROT_WRITE;
			//my_write(1,"eee\n",4);
		}
		if (pdr->p_flags & PF_X){
			protflags |= PROT_EXEC;
			//my_write(1,"rrr\n",4);
		}

		int pro_ret = my_mprotect(segment, rounded_len, protflags);
		if(pro_ret<0){
			//my_write(1,"mprotect wrong\n",15);
		}

		k = pdr->p_vaddr + pdr->p_memsz;
		if (k > brk_addr) brk_addr = k;
	}
	//my_write(1,"111",3);
	//如果有动态链接器，则需要先加载动态链接器
	if (interp){
		Elf64_Ehdr *junk_ehdr = 0;
		unsigned long sz_dummy;
		struct stat st;
		//my_write(1,&(((char *)mapped)[interp->p_offset]),strlen(&(((char *)mapped)[interp->p_offset])));
		int in = my_open(&(((char *)mapped)[interp->p_offset]),O_RDONLY,0);
		if(in<0){
			write(1,"open interp wrong\n", 18);
		}
		my_stat(in, &st);
		void *inmapped = my_mmap(0, st.st_size, PROT_READ|PROT_EXEC|PROT_WRITE, MAP_PRIVATE, in, 0);
		my_close(in);
		entry_point = load_elf(inmapped, 1, ldso_ehdr, &junk_ehdr);
		my_munmap(inmapped,st.st_size);
	}
	//my_write(1,"222",3);
	if (!anywhere)
		my_brk(ROUNDUP(brk_addr, 0x1000));

	return (void *)entry_point;
}

void us_exec(int argc, char **argv, char **env){
	char buf[1024], *p;
	char rbuf[2048];
	int cc, fd;
	fd = my_open("/proc/self/maps", 0, 0);
	//这里需要找到不包含路径的文件名，因为可能是使用相对地址比如./main
	char *s = strrchr(argv[0], '/');
	p = &buf[0];
	//清空地址空间
	while (0 < (cc = my_read(fd, rbuf, sizeof(rbuf)))){
		int i;
		for (i = 0; i < cc; ++i)
		{
			int c = rbuf[i];
			if ('\n' != c)
				*p++ = c;
			else {
				*p = '\0';
				if(strstr(buf, "libdl") || strstr(buf, s) || strstr(buf, "libc") || strstr(buf, "ld")){
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
					//打印maps文件的内容
					/*
					write(1,buf,p-buf);
					char ff[1] = {'\n'};
					write(1,ff,1);
					*/
					my_munmap((void *)low, high-low);
				}
				p = &buf[0];
			}
		}
	}
	my_close(fd);
	//map新的文件
	int newfd = my_open(argv[1],0,0);
	struct stat st;
	if(my_stat(newfd, &st)<0){
		my_write(1,"stat wrong\n",11);
	}
	void *new_file = my_mmap(0, st.st_size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (new_file == (void *) -1){
		my_write(1, "map wrong\n", 10);
	}
	int num,pos=0;
	while((num = my_read(newfd,buf,sizeof(buf)))>0){
		memcpy(new_file+pos,buf,num);
		pos += num;
	}
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)new_file;
	Elf64_Phdr *phdr = (Elf64_Phdr *)(ehdr + ehdr->e_phoff);
	int i,how_to_map=0;
	for (i = 0; i < ehdr->e_phnum; ++i){
		if (phdr[i].p_type == PT_LOAD && phdr[i].p_vaddr == 0){
			//如果代码的虚拟地址是0，则表示使用fPIC、PIE编译过了，是地址无关代码
			how_to_map = 1;
			//my_write(1,"www\n",4);
			break;
		}
	}
	Elf64_Ehdr *ldso_ehdr;
	void *entry_point = load_elf(new_file, how_to_map, &ehdr, &ldso_ehdr);
	my_munmap(new_file, st.st_size);

	//设置栈的参数
	/*
	栈底的布局应该是参数个数、参数数组、环境变量数组，辅助向量结构Elf64_auxv_t数组
	*/
	
	struct saved_block *argvb = save_argv(argc - 1, &argv[1]);
	struct saved_block *envb = save_argv(0, env);
	struct saved_block *elfauxvb = save_elfauxv(env);
	void *stack_bottom = stack_setup(argvb, envb, elfauxvb, ehdr, ldso_ehdr);
	
	__asm__ volatile("movq %0, %%rsp\n" :: "r"(stack_bottom));
	__asm__ volatile("jmp  *%0\n" :: "r" (entry_point));

	//pause，用来查看地址空间的情况
	my_pause();
	my_exit(0);
	/*
	char tmp[10];
	my_write(1,my_itoa(argc,tmp),1);
	my_write(1,argv[0],10);
	my_write(1,env[0],10);
	*/
}
