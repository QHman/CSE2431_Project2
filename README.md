# CSE2431_Project2
Linux Kernal
Project 2: Hijacking and manipulating system calls

Authors: Quinton Hiler & Andrew Cantor

Description: A system call hijacker that hooks onto open() and write(), stealing the information passed in.

Compile
Once the lkm and test folder are downloaded, use the MAKE file in the folder to compile the project.
Then use the ./run.sh command with 


NOTE: Program freezes when trying to unload the kernel module. Constant debugging would not reveal the issue.

Make File Commands and output:

lkm-syscall
===========

Linux Loadable Kernel Module System Call Hijacking 


    > ./run.sh 
    ┌──────────────────────────────┐
    │  Load Kernel Module syscall  │
    └──────────────────────────────┘
    # insmod syscall.ko

    ┌──────────────────────────────┐
    │           lsmod              │
    └──────────────────────────────┘
    $ lsmod | grep 'syscall'

    ┌──────────────────────────────┐
    │           Test               │
    └──────────────────────────────┘
    $ ./test 

    ┌──────────────────────────────┐
    │ Unload Kernel Module syscall │
    └──────────────────────────────┘
    # rmmod syscall

    ┌──────────────────────────────┐
    │           dmesg              │
    └──────────────────────────────┘
    $ dmesg
