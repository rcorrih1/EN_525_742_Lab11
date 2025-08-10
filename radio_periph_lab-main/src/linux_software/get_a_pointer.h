#ifndef GET_A_POINTER_H
#define GET_A_POINTER_H

#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>

volatile unsigned int * get_a_pointer(unsigned int phys_addr);

#endif