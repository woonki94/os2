# Lab 4: Preemptive Multitasking and Inter-Process Communication

In this lab, you will implement preemptive multitasking among multiple simultaneously active user-mode environments. The lab is divided into three parts, each building on the previous one to enhance the multitasking and communication capabilities of the JOS kernel.

## Lab Overview

### Part A: Multiprocessor Support and Round-Robin Scheduling
- **Objective**: Add support for multiprocessors, implement round-robin scheduling, and create basic environment management system calls.
- **Activities**:
  - Enable JOS to run on multiple processors.
  - Implement a round-robin scheduler to manage multiple user environments.
  - Add system calls for creating and destroying environments, and for allocating and mapping memory.

### Part B: Implementing `fork()`
- **Objective**: Implement a Unix-like `fork()` system call to allow a user-mode environment to create copies of itself.
- **Activities**:
  - Develop the `fork()` system call to duplicate an existing user environment.
  - Ensure that the new environment is an exact copy of the parent, with separate memory and execution state.

### Part C: Inter-Process Communication and Preemption
- **Objective**: Add support for inter-process communication (IPC) and hardware clock interrupts to enable preemption.
- **Activities**:
  - Implement IPC mechanisms to allow user-mode environments to communicate and synchronize.
  - Integrate hardware clock interrupts to support preemptive multitasking, allowing the kernel to preempt running environments.

## Lab Materials

- **Lab Description**: Detailed instructions and background information for each part of the lab are provided in the [Lab Description](./Lab4.pdf).
- **Lab Answers**: You can find the solutions and answers to the lab exercises in the [Lab Answers](./answers-lab4.md) document.

