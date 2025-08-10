#ifndef READ_FIFO_H
#define READ_FIFO_H

#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <stdint.h>
#include<stdbool.h>

unsigned int read_fifo(volatile unsigned int *fifo_base, int data_offset, int occupancy_offset);

#endif