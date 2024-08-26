// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>
#include <kern/pmap.h>


#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

// LAB 1: add your command to here...
static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
    { "backtrace", "Display stack backtrace", mon_backtrace },
    { "showmappings", "Display colored text", showmappings },
    { "changeperm", "Display colored text", change_perm },
    { "memdump", "Display colored text", mem_dump },
    { "continue", "Continue Execution from current location", mon_continue}


};

/***** Implementations of basic kernel monitor commands *****/

int
mon_continue(int argc, char **argv, struct Trapframe *tf)
{
    return 0;
}


int
showmappings(int argc, char **argv, struct Trapframe *tf)
{

    uintptr_t arg1 = ROUNDDOWN(strtol(argv[1], NULL, 16),PGSIZE);
    uintptr_t arg2 = ROUNDDOWN(strtol(argv[2], NULL, 16),PGSIZE);
    // cprintf("arg1 : %u, arg2: %u\n", arg1,arg2);
    // cprintf("arg1 : 0x%x, arg2: 0x%x\n", (unsigned int)arg1, (unsigned int)arg2);



    extern pde_t *kern_pgdir;

    for(uintptr_t p=arg1; p<=arg2; p+=4096){
        pte_t *pte  = pgdir_walk(kern_pgdir,(void*)p,0);
        cprintf("virtual address : 0x%x\t", (unsigned int)p );
        if(pte){
            cprintf("pysical address : %p\t", PTE_ADDR(*pte));
            cprintf("permissions: PTE_P(%x)\tPTE_U(%x)\tPTE_W(%x)\n", (*pte & PTE_P),(*pte & PTE_U),(*pte & PTE_W));
        }
        else 
            cprintf("no mapping\n");
        }

    return 0;
}


int
change_perm(int argc, char **argv, struct Trapframe *tf)
{
    uintptr_t addr = ROUNDDOWN(strtol(argv[1], NULL, 16), PGSIZE);
    pte_t *pte = pgdir_walk(kern_pgdir, (void *)addr, 0);

    if (pte != NULL)
    {
        int i = 0;
        uint32_t perm = 0;

        while (argv[3][i])
        {
            if (argv[3][i] == 'p' || argv[3][i] == 'P')
                perm |= PTE_P;
            else if (argv[3][i] == 'w' || argv[3][i] == 'W')
                perm |= PTE_W;
            else if (argv[3][i] == 'u' || argv[3][i] == 'U')
                perm |= PTE_U;
            else
            {
                cprintf("Invalid permission!\n");
                return -1;
            }

            i++;
        }

        if (strcmp(argv[2], "set") == 0)
            *pte |= perm;
        else if (strcmp(argv[2], "clear") == 0)
            *pte &= ~perm;
        else{
            cprintf("Invalid operation!\n");
            return -1;
        }

        cprintf("Permissions %s!\n", argv[2]);
        showmappings(argc, (char *[]){"", argv[1], argv[1]}, tf);
    }
    else
    {
        cprintf("Invalid address!\n");
        return -1;
    }

    return 0;
}
int
mem_dump(int argc, char **argv, struct Trapframe *tf)
{
    
    uintptr_t arg1 = ROUNDDOWN(strtol(argv[1], NULL, 16),16);
    uintptr_t arg2 = ROUNDDOWN(strtol(argv[2], NULL, 16),16);
    // cprintf("arg1 : %u, arg2: %u\n", arg1,arg2);
    // cprintf("arg1 : 0x%x, arg2: 0x%x\n", (unsigned int)arg1, (unsigned int)arg2);

    extern pde_t *kern_pgdir;

    for(uintptr_t p=arg1; p<=arg2; p+=16){
        pte_t *pte  = pgdir_walk(kern_pgdir,(void*)p,0);
        cprintf("virtual address : 0x%x\t", (unsigned int)p );
        if(pte){
            cprintf("pysical address : %p\t", PTE_ADDR(*pte));
            for (int j = 0; j < 0x10; j += 4) {
                cprintf("%08lx ", *(long *)(p + j));
            }
            cprintf("\n");
            continue;
        }
        else 
            cprintf("no mapping\n");
        }

    return 0;

}

int 
show(int argc, char **argv, struct Trapframe *tf)
{
    static const char * CSI = "\33[";
    cprintf("%s%s", CSI, "31m");
    cprintf("RED\n");
    cprintf("%s%s", CSI, "34m");
    cprintf("BLUE\n");
    cprintf("%s%s", CSI, "35m");
    cprintf("PURPLE\n");
    cprintf("%s%s", CSI, "33m");
    cprintf("YELLOW\n");
    cprintf("%s%s", CSI, "38;2;255;165;0m");
    cprintf("ORANGE\n");
    cprintf("%s%s", CSI, "0m");    
    return 0;
}

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(commands); i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// LAB 1: Your code here.
    // HINT 1: use read_ebp().
    unsigned int *ebp = ((unsigned int*)read_ebp());
	cprintf("Stack backtrace:\n");
    
    // HINT 2: print the current ebp on the first line (not current_ebp[0])
	while(ebp) {
		cprintf("ebp %08x ", ebp);
		cprintf("eip %08x args", ebp[1]);
        
		for(int i = 2; i < 7; i++)
			cprintf(" %08x", ebp[i]);
		cprintf("\n");

		unsigned int eip = ebp[1];
		struct Eipdebuginfo info;
		debuginfo_eip(eip, &info);
            
        cprintf("\t%s:%d: %.*s+%d\n", info.eip_file, info.eip_line, info.eip_fn_namelen, info.eip_fn_name, eip-info.eip_fn_addr);
        ebp = (unsigned int*)(*ebp);
	}
    
	return 0;
}



/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");


	if (tf != NULL)
		print_trapframe(tf);


	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}


