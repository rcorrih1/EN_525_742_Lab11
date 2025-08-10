#include "read_fifo.h"

unsigned int read_fifo(volatile unsigned int *fifo_base, int data_offset, int occupancy_offset)
{
	while (true)
	{
		if (*(fifo_base+occupancy_offset)>0)
		{
			return *(fifo_base+data_offset);
		}
	}
}
