### Q1. Assuming that the following JOS kernel code is correct, what type should variable `x` have, `uintptr_r` or `physaddr_t` , and why?

```
mystery_t x;
char * value = return_a_pointer();
*value = 10;
x = (mystery_t) value;
```

- According to lab description, when system has been changed to protected mode, there is no way to directly access physical memory. Which means that every pointers in C are virtual addresses. So pointer `value` is a char pointer which is pointing at virtual address. For this reason `x` should have the type `uintptr_r` which represents opaque virtual addresses.

### Q2. What entries (rows) in the page directory have been filled in at this point? What addresses do they map and where do they point? In other words, fill out this table as much as possible:

- According to this https://pdos.csail.mit.edu/6.828/2012/labguide.html link, we can see current page table by `info pg` command in QEMU. The table below is the result (erased result that shows PTE , only remaining PDE).
    
    
    | VPN Range | Entry | Flags | Physical Page |
    | --- | --- | --- | --- |
    | ef000-ef3ff | PDE[3bc] | -------UWP |  |
    | ef400-ef7ff | PDE[3bd] | -------U-P |  |
    | efc00-effff | PDE[3bf] | -------UWP |  |
    | f0000-f03ff | PDE[3c0] | ----A--UWP |  |
    | f0400-f7fff | PDE[3c1-3df] | ----A--UWP |  |
    | f8000-fffff | PDE[3e0-3fe] | -------UWP |  |
    | ffc00-fffff | PDE[3ff] | -------UWP |  |
- Converting the value v inside PDE[v] into decimal, we can get entry value to write. And Base Virtual Address can be drawn from VPN range. And where it points can be known by seeing the `memlayout.h`

| Entry | Base Virtual Address | Points to (logically) |
| --- | --- | --- |
| 1023 | 0xffc00000 | Page table for 4MB of phys memory |
| 1022 | 0xf8000000 | Page table for 4MB of phys memory |
| 960 | 0xf0000000 | KERNBASE and KSTACKTOP points this address. Which means that it is a starting point of 4MB physical memory. |
| 959 | 0xefc00000 | MMIOLIM points this address. Which is memory-mapped I/O.  |
| 957 | 0xef400000 | UVPT points this address. Which is User read-only virtual page table. |
| 956 | 0xef00000 | UPAGES points this address. Read-only copies of the Page structures. |

### Q3. We have placed the kernel and user environmental in the same address space. Why will user programs not be able to read or write the kernel’s memory?? What specific mechanisms protect the kernel memory?

- As learned in lecture 3, operating systems use a concept of privilege levels or protection rings. The kernel operates at the most privileged level (ring 0), while user programs operate at a less privileged level (ring 3).
- Descriptor Privilege Levels (DPL):
    - Every segment descriptor in GDT contains a two-bit field called the DPL
    - DPL specifies the privilege level required to access the segment.
    - DPL can have values 0, 1, 2, or 3, with 0 being the most privileged and 3 being the least privileged.
    - Kernel code and data reside in segments with DPL 0 (Ring 0).
    - User programs operate in segments with DPL 3 (Ring 3).
    - The CPU hardware ensures that access to memory is checked against the privilege level of the current executing code.
    - If a user program attempts to access kernel memory, the CPU will raise a privilege violation exception, preventing unauthorized access.
- `PTE_U` Flag bit
    - Moreover, as we learned in lecture 4 kernel memory is marked with page table entries that do not have the `PTE_U` flag set. This means that the kernel memory is not accessible from user mode.
    - When a user program tries to access kernel memory, the hardware checks the `PTE_U` flag in the page table entry.
    - If `PTE_U` is not set, the hardware raises a page fault, preventing the unauthorized access attempt.
- But I think for this question, `PDE_U` and `PTE_U` is more appropriate answer, since it is asking why user cannot access kernel memory even though they are in same address space. And to do that we have set memory above `ULIM` as kernel only memory, `[UTOP,ULIM]`  as user and kernel memory, under `UTOP` as user memory using `boot_map_region` function. DPL is more related to controlling access to segments in GDT.

### Q4. What is the maximum amount of physical memory that this operating system can support? Why?

- To find the maximum amount of physical memory that the operating system can support, we need to consider the size of the physical page and the number of physical pages supported by the system. We know that the physical page size is 4KB (or 4096 bytes). By looking at the table from question number 2, starting from 960 and all the way to 1023, the machine has 64 page directory entries pointing to page tables.
- Each Page Entries hold 1024 entries pointing to a page.
- 64 * 1024 * 4096 = 264241152
- Which is about 256MB

### Q5. How much space overhead is there for managing memory, if we actually had the maximum amount of physical memory? How it this overhead broken down?

To calculate the space overhead for managing memory, we need to consider the space required for page directory entries and page table entries.

Given that the page size is 4KB (4096 bytes) and each page table entry and page directory entry is 4 bytes, we can calculate the total space overhead as follows:

- Size for Page Directory Entries:
    - 1024 * 4bytes = 4KB
- Size of Page Table Entries:
    - 1024(Number of PT) * 1024 * 4MB = 4MB
- Total Space Overhead:
    - Total space overhead = Size for Page Directory Entries + Size of Page Table Entries
    = 4MB + 4KB = 4100KB

Therefore, the space overhead for managing memory, if we actually had the maximum amount of physical memory, is 4100KB.

### Q6. Revisit the page table setup in `kern/entry.s`  and `kern/entrypgdir.c` . Immediately after we turn on paging, `EIP` is still a low number (a little over 1MB). At what point do we transition to running at an `EIP` above `KERNBASE`? What makes it possible for us to continue executing at a low `EIP` between when we enable paging and when we begin running at an `EIP` above `KERNBASE`? Why is this transition necessary?

In the JOS kernel, the transition to running at an EIP above KERNBASE occurs during the initialization of the kernel's page directory (`kern_pgdir`).

- Enabling Paging:
    - Initially, when paging is enabled in `kern/entry.S`, the page directory base register (cr3) is loaded with the physical address of `kern_pgdir`. This establishes the kernel's page directory as the active page directory.
- Page Table Setup:
    - Before the page directory is fully initialized, the kernel is still running while virtual addresses are equal to physical addresses.
    - The kernel's page directory is set up to map the entire physical memory range with a 4 MB large page, and kernel text and data are mapped into the kernel's virtual address space starting at `KERNBASE`.
    - After setting up the kernel's page directory, the page table for the virtual address `KERNBASE` is recursively mapped to the page directory itself.
- Transition to Running at an EIP above KERNBASE:
    - Once the kernel's page directory is fully initialized, the processor is set to jump to the high virtual address entry point of the kernel (`entry_pgdir`).
    - From this point onward, the processor starts executing at virtual addresses above `KERNBASE`.
- The transition to running at an EIP above `KERNBASE` is necessary because:
    - It allows the kernel to utilize the MMU and paging  to protect the kernel's memory and to map physical memory to different virtual addresses.
    - It provides a separation between kernel space and user space, which prevents user programs from accessing or modifying kernel memory.
- During the time between enabling paging and transitioning to running at an EIP above `KERNBASE`, the kernel is still executing while virtual addresses are the same as physical addresses. This transition is necessary because it allows the kernel to fully utilize paging and virtual memory features to ensure memory protection and isolation between kernel and user space.