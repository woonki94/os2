### Q1. Compare kern/mpentry.S side by side with boot/boot.S. Bearing in mind
that kern/mpentry.S is compiled and linked to run above KERNBASE just like everything else in the kernel, what is the purpose of macro MPBOOTPHYS? Why is it necessary in kern/mpentry.S but not in boot/boot.S? In other words, what could go wrong if it were omitted in kern/mpentry.S? Hint: recall the differences between the link address and the load address that we have discussed in Lab 1.

- In `kern/mpentry.S`, the macro `MPBOOTPHYS` is used to calculate the physical address of the entry point for the additional CPUs during the boot process. This macro ensures that the code executed by the additional CPUs starts at the correct physical address.
- The purpose of `MPBOOTPHYS` is necessary in `kern/mpentry.S` because the kernel is loaded into physical memory starting at `KERNBASE`, but during boot, the kernel is still executing in low memory. Therefore, `MPBOOTPHYS` is needed to calculate the correct physical address for the entry point of the secondary CPUs, ensuring that the entry code for APs can correctly reference its own code and data when running from a physical address.
- In contrast, `boot/boot.S` does not need `MPBOOTPHYS` because BIOS operates it in real mode, where physical and linear addresses are the same.
- If `MPBOOTPHYS` were omitted, the additional CPUs would attempt to execute code at the wrong physical address, leading to incorrect behavior.

### Q2. It seems that using the big kernel lock guarantees that only one CPU can run the kernel code at a time. Why do we still need separate kernel stacks for each CPU? Describe a scenario in which using a shared kernel stack will go wrong, even with the protection of the big kernel lock.

- By having separate kernel stacks for each CPU, the kernel ensures that each CPU has its own isolated stack space, preventing interference and ensuring thread safety even when the big kernel lock is in use.
- One scenario where using a shared kernel stack would go wrong is in the case of interrupt handling. If multiple CPUs were using the same kernel stack and an interrupt occurred while the kernel was running on one CPU, it could overwrite critical data on the shared stack being used by another CPU, leading to corruption or incorrect behavior.

### Q3. In your implementation of env_run() you should have called lcr3(). Before and after the call to lcr3(), your code makes references (at least it should) to the variable e, the argument to env_run. Upon loading the %cr3 register, the addressing context used by the MMU is instantly changed. But a virtual address (namely e) has meaning relative to a given address context–the address context specifies the physical address to which the virtual address maps. Why can the pointer e be dereferenced both before and after the addressing switch?

- The virtual address space layout for all environments is consistent above `UTOP` and below `UVPT`. This includes the kernel portion of the address space.
- The kernel maintains identical mappings in each environment's page tables for addresses above `KERNBASE`. Thus, the pointer `e`, which is in the kernel space, remains valid across context switches.

### Q4. Whenever the kernel switches from one environment to another, it must ensure the old environment’s registers are saved so they can be restored properly later. Why? Where does this happen?

- During a context switch, the kernel saves the CPU state (including trap frame and registers) of the current environment onto its kernel stack.
- In `trap.c`, the trap frame is copied and stored on the stack when a trap or interrupt occurs. When `env_run()` is called to switch to a new environment, the function `env_pop_tf()` restores the saved trap frame, restoring the CPU state of the environment being resumed.