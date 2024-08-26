1. At what point does the processor start executing 32-bit code? What exactly causes the switch from 16- to 32-bit mode?
    - In /obj/boot/boot.asm file code, the code below causes the switch from 16- to 32-bit mode.
    
    ```
    # Jump to next instruction, but in 32-bit code segment.
    # Switches processor into 32-bit mode.
    ljmp    $PROT_MODE_CSEG, $protcseg
        7c2d:	ea                   	.byte 0xea
        7c2e:	32 7c 08 00          	xor    0x0(%eax,%ecx,1),%bh
    
    ```
    
2. What is the last instruction of the boot loader executed, and what is the first instruction of the kernel it just loaded?
    - Last instruction of the boot loader executed : To see the last instruction boot loader executed, I analyzed /boot/main.c code.
    Then found that code below was the last instruction bootmain() executed.
        
        ```
        ((void (*)(void)) (ELFHDR->e_entry))();
        ```
        
        In /obj/boot/boot.asm file I could find equivalent code below.
        
        ```
        ((void (*)(void)) (ELFHDR->e_entry))();
        7d61:	ff 15 18 00 01 00    	call   *0x10018
        ```
        
    - First instruction of the kernel it just loaded: Code below located at /obj/kern/kernel.asm
        
        ```
        entry:
        movw	$0x1234,0x472			# warm boot
        ```
        
3. Where is the first instruction of the kernel?
    - The address of the first instruction of the kernel must be at `0x0010000c` ,since the boot loader’s last command was calling `0x10018` , and in here it calls instruction in `0x0010000c`
        
        ```
        7d61:	ff 15 18 00 01 00    	call   *0x10018
        ```
        
        ```
        >>> x/10x 0x10018
        0x10018:	0x0010000c	0x00000034	0x00014338	0x00000000
        0x10028:	0x00200034	0x00280003	0x000a000b	0x00000001
        0x10038:	0x00001000	0xf0100000
        ```
        
4. How does the boot loader decide how many sectors it must read in order to fetch the entire kernel from disk? Where does it find this information?
    
    ```
    // load each program segment (ignores ph flags)
    ph = (struct Proghdr *) ((uint8_t *) ELFHDR + ELFHDR->e_phoff);
    eph = ph + ELFHDR->e_phnum;
    for (; ph < eph; ph++)
    		// p_pa is the load address of this segment (as well
    		// as the physical address)
    		readseg(ph->p_pa, ph->p_memsz, ph->p_offset);
    ```
    
    It loops through program header and fetches each segment one by one, ultimately fetching entire kernel from disk.
    
5. From the output, explain how the address values of pointers a, b, and c in line 1 change to the values shown in line 6.
    - Line 1 : `a = 0x7ffe67d50750, b = 0xa78010, c = 0x40074d`
        
        ```
        int a[4]; //initiallizing array which has same meaning as int *a, memory allocated in the stack
        int *b = malloc(16); // Allocating memory using malloc, in heap
        int *c; // pointer not initialized
        printf("1: a = %p, b = %p, c = %p\n", a, b, c); //prints out physical memory of each pointer
        ```
        
        It is printing out a, b, c’s physical addresses.
        
    - Line 6: `a = 0x7ffe67d50750, b = 0x7ffe67d50754, c = 0x7ffe67d50751`
        
        a hasn’t been modified so it is pointing at same address as line 1.
        
        b is pointing a[1] because of the code below, making it (physical address that a points) + 4(since it is integer array).
        
        ```
        b = (int *) a + 1;
        ```
        
        c is pointing address has been modified by the code below.
        
        ```
        c = a; 
        ...
        c = c + 1; 
        ...
        c = (int *) ((char *) c + 1);
        ...
        c = (int *) ((char *) a + 1);
        ...
        ```
        
        `c=a` makes c points same address as a does. which is `0x7ffe67d50750`
        
        `c=c+1` makes c to point a’s physical address +4, which is `0x7ffe67d50754` 
        
        `c = (int *) ((char *) c + 1)` makes c to point (0x7ffe67d50754 + 1) since type of c is casted to char and 1 is added, which increments address by 1 byte not 1 integer. So now c is pointing to `0x7ffe67d50755`
        
        `c = (int *) ((char *) a + 1);` makes c to point (0x7ffe67d50750 + 1) = **`0x7ffe67d50751`**
        
6. How do we get all the values in lines 2 through 4?
    - Line 2
        
        ```
        c = a; //allocating same physical memory of a to c
        for (i = 0; i < 4; i++)
        a[i] = 100 + i; // putting values in memory. a[0] = 100, a[1] = 101, a[2] = 102, a[2] = 103
        c[0] = 200; // putting values in c[0] which is same as a[0]. a[0] = c[0] = 200
        printf("2: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
        a[0], a[1], a[2], a[3]);
        ```
        
        c points same address as a[0], and by using for loop program has putted values in array a (a[0] = 100,a[1] = 101, a[2] = 102, a[3] = 103).
        
        and by doing `c[0] = 200`, now the value of a[0] has changed into 200, since c points same physical address with a.
        
        So values in a will be a[0] = 200, a[1] = 101, a[2] = 102, a[3] = 103.
        
    - Line 3
        
        ```
        c[1] = 300; // c[1] = a[1] = 300
        *(c + 2) = 301;// *(c + 2) = c[2] = a[2] = 301
        3[c] = 302; // 3[c] = *(3 + c) = *(3 + c) = c[3] = a[3] = 302
        printf("3: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
        a[0], a[1], a[2], a[3]);
        ```
        
        c[1] = a[1] = 300
        
        *(c + 2) = c[2] = a[2] = 301
        
        3[c] = *(3 + c) = *(3 + c) = c[3] = a[3] = 302
        
        a[0] hasn’t been modified, same as line 2.
        
        So values in a will be a[0] = 200, a[1] = 300, a[2] = 301, a[3] = 302.
        
    - Line 4
        
        ```
        c = c + 1; 
        *c = 400;
        printf("4: a[0] = %d, a[1] = %d, a[2] = %d, a[3] = %d\n",
        a[0], a[1], a[2], a[3]);
        ```
        
        Because of `c = c+1` c is now pointing at a[1].
        
        *c = a[1] = 400
        
        Other values in array hasn’t been modified.
        
        So values in a will be a[0] = 200, a[1] = 400, a[2] = 301, a[3] = 302.
        
7. Why are the values printed in line 5 seemingly corrupted?
    - It is due to the code below
        
        ```
        c = (int *) ((char *) c + 1);
        *c = 500;
        ```
        
        As I have explained in number 6, after `c = (int *) ((char *) c + 1);` this code c is pointing at `0x7ffe67d50755`.
        
        And it is in between a[1](0x7ffe67d50754) and a[2](0x7ffe67d50758).
        
        `*c = 500` this code will put value to the address `0x7ffe67d50755` and will affect values in *(a+1) and *(a+2), messing up both a[1] and a[2] values.
        
8. Why are they different?
    - When breakpoint is set to 0x7c00 which is when BIOS enters the bootloader the result of x/8x 0x10000c is
        
        ```
        0x10000c:	0x00000000	0x00000000	0x00000000	0x00000000
        0x10001c:	0x00000000	0x00000000	0x00000000	0x00000000
        ...
        ```
        
    - When breakpoint is set to 0x10000c, which is when boot loader enters the kernel, the result of x/8x 0x10000c is
        
        ```
        0x10000c:	0x7205c766	0x34000004	0x0000b812	0x220f0011
        0x10001c:	0xc0200fd8	0x0100010d	0xc0220f80	0x10002fb8
        ...
        ```
        
    - It is different because when BIOS enters the boot loader, kernel is not loaded yet. Then, after bootloader enters the kernel, the bootloader loads kernel to `0x00100000.`
        
        ```
        >>> x/10i 0x0010000c
        => 0x10000c:	movw   $0x1234,0x472
           0x100015:	mov    $0x110000,%eax
           0x10001a:	mov    %eax,%cr3
           0x10001d:	mov    %cr0,%eax
           0x100020:	or     $0x80010001,%eax
           0x100025:	mov    %eax,%cr0
           0x100028:	mov    $0xf010002f,%eax
           0x10002d:	jmp    *%eax
           0x10002f:	mov    $0x0,%ebp
           0x100034:	mov    $0xf0110000,%esp
        >>>
        ```
        
    
9. What is there at the second breakpoint? (You do not really need to use QEMU to answer this question.)
    - Instructions of Kernel.
    
    ```
    0x10000c:	0x7205c766	0x34000004	0x0000b812	0x220f0011
    0x10001c:	0xc0200fd8	0x0100010d	0xc0220f80	0x10002fb8
    ...
    ```
    
10. What is the first instruction after the new mapping is established that would fail to work properly if the mapping weren’t in place? Comment out the movl %eax, %cr0 in kern/entry.S, trace into it, and see if you were right.
    - First I have set break point at the end of the bootloader, and si to next instruction to load kernel.
    
    ```
    >> b* 0x7d61
    >> c
    >> si
    ```
    
    Then step over until I see `movl %eax, %cr0` and examined the memory at  `0x100000` , `0xf0100000` .
    
    ```
    >>> x/8w 0x00100000
    0x100000:	0x1badb002	0x00000000	0xe4524ffe	0x7205c766
    0x100010:	0x34000004	0x0000b812	0x220f0011	0xc0200fd8
    >>> x/8w 0xf0100000
    0xf0100000 <_start+4026531828>:	0x00000000	0x00000000	0x00000000	0x00000000
    0xf0100010 <entry+4>:	0x00000000	0x00000000	0x00000000	0x00000000
    ```
    
    Then after a single step over, examined again.
    
    ```
    >>> x/8w 0x00100000
    0x100000:	0x1badb002	0x00000000	0xe4524ffe	0x7205c766
    0x100010:	0x34000004	0x0000b812	0x220f0011	0xc0200fd8
    >>> x/8w 0xf0100000
    0xf0100000 <_start+4026531828>:	0x1badb002	0x00000000	0xe4524ffe	0x7205c766
    0xf0100010 <entry+4>:	0x34000004	0x0000b812	0x220f0011	0xc0200fd8
    ```
    
    I see that instructions at `0x00100000` has loaded into `0xf0100000` , enabling paging. If `movl %eax %cr0` is commented out `jmp *%eax` won’t work, since it is not inside RAM.
    
    - After commenting out `movl %eax %cr0` the first instruction is
    
    ```
    69 relocated:
    70
    71     # Clear the frame pointer register (EBP)
    72     # so that once we get into debugging C code,
    73     # stack backtraces will be terminated properly.
    74     movl    $0x0,%ebp            # nuke frame pointer
    75
    76     # Set the stack pointer
    77     movl    $(bootstacktop),%esp
    78
    79     # now to C code
    ```
    
    with error message in console
    
    ```
    qemu: fatal: Trying to execute code outside RAM or ROM at 0xf010002c
    ```
    
11. Explain the interface between kern/printf.c and kern/console.c. Specifically, what
function does kern/console.c export? How is this function used by kern/printf.c?
    - kern/console.c exports `cputchar` function. It is called inside `putch` function and `putch` is used in `vcprintf` which is called in `cprintf` function which prints out the console output for the kernel.

12. Explain the following from kern/console.c:
    
    ```
    1	if (crt_pos >= CRT_SIZE) {
    2			int i;
    3			memcpy(crt_buf, crt_buf + CRT_COLS, (CRT_SIZE - CRT_COLS) * sizeof(uint16_t));
    4			for (i = CRT_SIZE - CRT_COLS; i < CRT_SIZE; i++)
    5				crt_buf[i] = 0x0700 | ' ';
    6			crt_pos -= CRT_COLS;
    7	}
    ```
    
    - This code is to make additional space when screen is full.
        
        Line 1 ⇒ checking if the current position `crt_pos` is greater than current size `CRT_SIZE` 
        
        Line 3 ⇒  by memcpy, it copies the data from the second row and to the end of the buffer. So now we can delete first row and print from second row.
        
        Line 4,5 ⇒ by looping, it fills the bottom line with spaces.
        
        Line 6 ⇒ make current position `crt_pos` to the last line.
        

13. In the call to cprintf(), to what does fmt point? To what does ap point?
    - To see what `fmt` and `ap` does, I have added the code below to `kern/init.c`
    
    ```
    int x = 1, y = 3, z = 4;
    cprintf("x %d, y %x, z %d\n", x, y, z);
    ```
    
    I can see that 
    
    ```
    144     int x = 1, y = 3, z = 4;
    145     cprintf("x %d, y %x, z %d\n", x, y, z);
    146 f01000de:   c7 44 24 0c 04 00 00    movl   $0x4,0xc(%esp)
    147 f01000e5:   00
    148 f01000e6:   c7 44 24 08 03 00 00    movl   $0x3,0x8(%esp)
    149 f01000ed:   00
    150 f01000ee:   c7 44 24 04 01 00 00    movl   $0x1,0x4(%esp)
    151 f01000f5:   00
    152 f01000f6:   c7 04 24 54 19 10 f0    movl   $0xf0101954,(%esp)
    153 f01000fd:   e8 4f 08 00 00          call   f0100951 <cprintf>
    ```
    
    this code has been generated in /obj/kern/kernel.asm. So to see the value of `fmt` , I have set break point to the start of <cprintf> by `b *0xf01000fd`, and skipped to the next instruction to the point fmt enters into stack.
    
    ```
    arg fmt = 0xf0101954
    ```
    
    Then examined the address. 
    
    ```
    >>> x/4x 0xf0101954
    0xf0101954:	0x64252078	0x2079202c	0x202c7825	0x6425207a
    ```
    
    Convert those hexadecimal ascii to char ⇒ `d% x y , ,x%d% z`. It is in little-endian so if I convert this to the form to readable form, it is equivalent to “x %d, y %x, z %d”.
    
    In same way, when examining `ap` address I got :
    
    ```
    >>> x/4x 0xf010ffe4
    0xf010ffe4:	0x00000001	0x00000003	0x00000004	0x00000000
    ```
    
    which is equivalent to “1 3 4” the value of arguemnets.
    
    From this analysis I can see that `fmt` holds strings from `cprintf` and `ap` holds value of the argumnents.
    
14. List (in order of execution) each call to cons_putc, va_arg, and vcprintf.
For cons_putc, list its argument as well. For va_arg, list what ap points to before and
after the call. For vcprintf list the values of its two arguments.
    
    ```
    cprintf (fmt = 0xf0101b54)
    >>x/10x 0xf0101b54
    0xf0101b54:	0x64252078	0x2079202c	0x202c7825	0x6425207a
    ```
    
    which means `x %d, y %x, z %d` 
    
    ```
    vcprintf (fmt = 0xf0101b54, ap = 0xf010ffe4)
    >>x/10x 0xf010ffe4
    0xf010ffe4:	0x00000001	0x00000003	0x00000004	0x00000000
    ```
    
    which means `1` `3` `4` 
    
    ```
    cons_putc 
    arg c = 120 => x
    
    cons_putc 
    arg c = 32 => space
    
    va_arg(*ap, int) 
    Before="\\001”, After="\\003"
    
    cons_putc 
    arg c = 49 => 1
    
    cons_putc 
    arg c = 44 => ,
    
    cons_putc 
    arg c = 32 => space
    
    cons_putc 
    arg c = 121 => y
    
    cons_putc 
    arg c = 32 => space
    
    va_arg(*ap, int) 
    Before="\\001”, After="\\003"
    
    cons_putc 
    arg c = 51 => 3
    
    cons_putc 
    arg c = 44 => ,
    
    cons_putc 
    arg c = 32 => space
    
    cons_putc 
    arg c = 122 => z
    
    cons_putc 
    arg c = 32 => space
    
    va_arg(*ap, int) 
    Before="\\004”, After="T\\034\\020?\\214_\\021??\\027\\020??_\\021??\\027\\020?_\\021?_\\021?"
    
    cons_putc 
    arg c = 52 => 4
    
    cons_putc
    arg c = 32 => \n
    
    ```
    
15. What is the output? Explain how this output is arrived at in the step-by-step
manner of the previous exercise. Here’s an ASCII table (or type man ascii) that
maps bytes to characters.
    - Running code below, the output is `He110 World` .
    
    ```
    unsigned int i = 0x00646c72;
    cprintf("H%x Wo%s", 57616, &i);
    ```
    
    `57616` in hexadecimal is **`e110`** 
    
    System reads `0x00646c72` in little-endian way. 72 ⇒ r, 6c⇒ l, 64 ⇒ d, 00 ⇒ NULL , which is `rld`. 
    
    So by summing up every value the result is `He110 World` 
    
16. The output depends on the fact that the x86 is little-endian. If the x86 were
instead big-endian what would you set i to in order to yield the same output?
Would you need to change 57616 to a different value?
    - `i` needs to be changed to `726c6400` , and `57616` doesn’t need to be changed.

17. In the following code, what is going to be printed after 'y='? (note: the answer is
not a specific value.) Why does this happen?
    - I  got `x=3 y=1600` for the result.
        
        To see why this happen, I have set break points to the address where `cprintf` is called, which is seen in code below that the address is `f01000ed` .
        
        ```
        146     cprintf("x=%d y=%d", 3);
        147 f01000de:   c7 44 24 04 03 00 00    movl   $0x3,0x4(%esp)
        148 f01000e5:   00
        149 f01000e6:   c7 04 24 34 19 10 f0    movl   $0xf0101934,(%esp)
        150 f01000ed:   e8 4f 08 00 00          call   f0100941 <cprintf>
        ```
        
        And skipped instruction until I see `arg ap = 0xf010ffe4` in stack.
        
        ```
        >>> x/4x 0xf010ffe4
        0xf010ffe4:	0x00000003	0x00000640	0x00000000	0x00000000
        ```
        
        The function first reads argument which is `3` so that the `x=3` is printed out as expected, then the program searches for the next argument on the stack which is some random value.
        
    
18. How would you have to change cprintf or its interface so that it would still be
possible to pass it a variable number of arguments?
    - Append an extra integer at the end of the argument list, which specifies the total number of arguments being passed.

19. How many 32-bit words does each recursive nesting level of test_backtrace push on
the stack, and what are those words?
    - Each recursive nesting level of test_backtrace pushes 8 words on the stack.
    
    ```
    test_backtrace(int x)
    {
    	f010003d:	55                   	push   %ebp
    	f010003e:	89 e5                	mov    %esp,%ebp
    	f0100040:	53                   	push   %ebx
    	f0100041:	83 ec 14             	sub    $0x14,%esp
    	f0100044:	8b 5d 08             	mov    0x8(%ebp),%ebx
    		cprintf("entering test_backtrace %d\n", x);
    	f0100047:	89 5c 24 04          	mov    %ebx,0x4(%esp)
    	f010004b:	c7 04 24 00 1b 10 f0 	movl   $0xf0101b00,(%esp)
    	f0100052:	e8 a1 0a 00 00       	call   f0100af8 <cprintf>
    		if (x > 0)
    	f0100057:	85 db                	test   %ebx,%ebx
    	f0100059:	7e 0d                	jle    f0100068 <test_backtrace+0x2b>
    			test_backtrace(x-1);
    	f010005b:	8d 43 ff             	lea    -0x1(%ebx),%eax
    	f010005e:	89 04 24             	mov    %eax,(%esp)
    	f0100061:	e8 d7 ff ff ff       	call   f010003d <test_backtrace>
    	f0100066:	eb 1c                	jmp    f0100084 <test_backtrace+0x47>
    	....
    }
    
    ```
    
    test_backtrace function pushes `ebp`, `ebx`, `eax`, and `eip` to the stack. And in between pushing `ebx` and `eax` test_backtrace function moves stack pointer making 12bytes empty space in stack. I am guessing it is because when calling cprintf, there’s a space needed to put arguments in. 
    
    So if I sum up the stack space used, it is 32Bytes, which is equivalent to 8-words.