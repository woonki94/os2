### Q1. What is the purpose of having an individual handler function for each exception/interrupt? (i.e., if all exceptions/interrupts were delivered to the same handler, what feature that exists in the current implementation could not be provided?)

- Having individual handler functions enables the operating system to customize how it handles each exception or interrupt. Since each exception or interrupt may require a different response, having separate handler functions allows the operating system to provide specific and appropriate actions for each type of exception or interrupt.
- For example, a page fault may require the operating system to allocate memory, while a divzero exception require terminating the process.
- If all exceptions and interrupts were delivered to the same handler, the operating system would not be able to differentiate between different types of exceptions and interrupts, and thus would not be able to provide the specific and customized responses necessary for proper system operation.

### Q2. Did you have to do anything to make the user/softint program behave correctly? The grade script expects it to produce a general protection fault (trap 13), but softint’s code says `int $14`. Why should this produce interrupt vector 13? What happens if the kernel actually allows softint’s `int $14` instruction to invoke the kernel’s page fault handler (which is interrupt vector 14)?

- There was no action taken to make softint program behave correctly
- In the softint program, it calls interrupt 14, which is a page fault.
- However, page fault’s `dpl` is set to `0` , which means that only kernel can generate this interrupt.

```cpp
    SETGATE(idt[T_PGFLT], 0, GD_KT, t_pgflt, 0);
```

- When the kernel receives the **`int $14`** instruction generated from the user program, it recognizes it as an invalid instruction and handles it by invoking the general protection fault handler, which is interrupt vector 13.
- If the kernel were to allow the **`int $14`** instruction from the softint program to invoke the kernel’s page fault handler, it would lead to an infinite loop. This is because the page fault handler would attempt to access the same invalid instruction again, causing another page fault, and so on. This would result in the system being stuck in an infinite loop of page faults.

### Q3. The break point test case will either generate a break point exception or a general protection fault depending on how you initialized the break point entry in the IDT (i.e., your call to SETGATE from trap_init). Why? How do you need to set it up in order to get the breakpoint exception to work as specified above and what incorrect setup would cause it to trigger a general protection fault?

- The Break Point Exception is a special interrupt used for debugging purposes. It should be accessible from user mode, so its Descriptor Privilege Level (DPL) needs to be set to 3. If the DPL is set to 0, it won't allow user programs to invoke this interrupt, triggering a general protection fault instead.
- To set up the Break Point Exception to work as specified:
    
    ```c
    SETGATE(idt[T_BRKPT], 0, GD_KT, t_brkpt, 3);
    ```
    
- Incorrect setup causing a general protection fault:
    
    ```c
    SETGATE(idt[T_BRKPT], 0, GD_KT, t_brkpt, 0);
    ```
    
- Setting the DPL to 0 would prevent user programs from invoking this interrupt, causing a general protection fault instead.

### Q4. What do you think is the point of these mechanisms, particularly in light of what the user/softint test program does?

- The primary point of these mechanisms is to ensure system security and stability by enforcing privilege levels and separating user and kernel memory spaces. Just like we did for the DPL in GDT to determine the privilege level of system resources, and the Page Table Entry PTE_U in page tables to separate kernel and user memory.
- By setting the DPL correctly in the Interrupt Descriptor Table (IDT), we control which interrupts and exceptions can be triggered by user-level programs. This prevents user programs from accessing privileged kernel resources directly.
- The **`softint`** test program demonstrates the importance of correctly setting the DPL. If the DPL is not set properly, user-level programs could potentially execute privileged instructions, leading to security vulnerabilities and system instability. Therefore, it's crucial to set the DPL appropriately to restrict access to kernel-level resources.