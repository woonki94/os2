# Lab 2: Memory Management

In this lab, you will implement the memory management system for your operating system. Memory management is a crucial aspect of operating systems, involving both physical and virtual memory management.

## Lab Overview

### Part 1: Physical Memory Allocator
- **Objective**: Develop a physical memory allocator for the kernel.
- **Activities**:
  - Implement a memory allocator that operates in units of 4096 bytes, known as pages.
  - Maintain data structures that track which physical pages are free, which are allocated, and how many processes are sharing each allocated page.
  - Write routines to allocate and free pages of memory.

### Part 2: Virtual Memory
- **Objective**: Set up virtual memory to map virtual addresses used by the kernel and user software to physical memory addresses.
- **Activities**:
  - Understand how the x86 hardware’s Memory Management Unit (MMU) uses page tables to perform address mapping.
  - Modify JOS to configure the MMU’s page tables according to the provided specifications.

## Lab Materials

- **Lab Description**: Detailed instructions and background information for each part of the lab are provided in the [Lab Description](./Lab2.pdf).
- **Lab Answers**: You can find the solutions and answers to the lab exercises in the [Lab Answers](./answers-lab2.md) document.

