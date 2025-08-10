#include "get_a_pointer.h"
#include "read_fifo.h"
#include "send_udp.h"

#define _BSD_SOURCE

#define RADIO_PERIPH_ADDRESS 0x43c00000
#define RADIO_TUNER_FAKE_ADC_PINC_OFFSET 0
#define RADIO_TUNER_TUNER_PINC_OFFSET 1
#define RADIO_TUNER_CONTROL_REG_OFFSET 2
#define RADIO_TUNER_TIMER_REG_OFFSET 3

#define FIFO_ADDRESS 0x43c10000
#define FIFO_RXD_DATA_OFFSET 8
#define FIFO_RXD_OCCUPANCY_OFFSET 7

#define BUFFER_SIZE 256
#define UDP_PORT 25344

// 1024 byte buffer
uint32_t buffer[BUFFER_SIZE];

int main(int argc, char *argv[])
{
	// check correct inputs
	if (argc != 2) {
		fprintf(stderr, "Usage: %s [ip address]\n", argv[0]);
	return EXIT_FAILURE;
    }
	
	// define ip address
	const char *ip_address = argv[1];
	
	// get a pointer to the radio and fifo base address
    volatile unsigned int *my_radio = get_a_pointer(RADIO_PERIPH_ADDRESS),
		*my_fifo = get_a_pointer(FIFO_ADDRESS);
		
	// ensure radio is not in reset (LSB=0)
	*(my_radio+RADIO_TUNER_CONTROL_REG_OFFSET) = 0x0000;
	
	// Continuously read FIFO and output via UDP
	while(true)
	{
		for (int i=0; i<BUFFER_SIZE; i++)
		{
			buffer[i] = read_fifo(my_fifo, FIFO_RXD_DATA_OFFSET, FIFO_RXD_OCCUPANCY_OFFSET);
		}
		send_udp(ip_address, buffer, UDP_PORT);
	}
	
    return 0;
}
