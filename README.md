Project Name: OS
Description:
This project implements a multithreaded operating system kernel for RISC-V processors, developed on Linux using CLion. 
It includes essential features such as memory allocation, thread management, semaphores, time-sharing, asynchronous preemption, and console I/O handling. 
This project was independently developed as part of the course "Operating Systems 1" at the University of Belgrade, Faculty of Electrical Engineering.

Features:
- Memory allocation and deallocation (First Fit/Best Fit algorithm).
- Thread creation, dispatch, and termination.
- Semaphore operations for synchronization.
- Time-sharing for threads using a timer interrupt.
- Console I/O with buffer management.

Usage:
- Build the project: make all
- Run the project: make qemu and enter the number of test case you want to run from 1 to 7
- The kernel runs in the QEMU virtual machine, simulating the RISC-V processor.
- Implement custom test cases in the src directory.

Project Structure:
- src/: Contains implementation files (.cpp and .s).
- inc/: Contains header files (.h and .hpp).
- Makefile: Defines build and execution targets.
