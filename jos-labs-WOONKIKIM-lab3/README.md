# Lab 3: User Environments and System Calls

In this lab, you will implement the basic kernel facilities necessary to create and manage protected user-mode environments (i.e., “processes”). You will enhance the JOS kernel to manage user environments, load and run user programs, and handle system calls and exceptions.

## Lab Overview

### Part 1: User Environment Setup
- **Objective**: Set up the data structures needed to track user environments.
- **Activities**:
  - Implement data structures to manage user environments in the kernel.
  - Create and initialize a single user environment.

### Part 2: Program Loading
- **Objective**: Load a program image into the user environment and start executing it.
- **Activities**:
  - Load a user program into memory.
  - Set up the execution context for the user environment and start running the program.

### Part 3: System Calls and Exception Handling
- **Objective**: Enable the JOS kernel to handle system calls and other exceptions from user environments.
- **Activities**:
  - Implement system call handling in the kernel.
  - Handle exceptions and faults caused by the user environment.

## Lab Materials

- **Lab Description**: Detailed instructions and background information for each part of the lab are provided in the [Lab Description](./Lab3.pdf).
- **Lab Answers**: You can find the solutions and answers to the lab exercises in the [Lab Answers](./answers-lab3.md) document.

