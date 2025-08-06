#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#define _BSD_SOURCE

#define RADIO_TUNER_FAKE_ADC_PINC_OFFSET 0
#define RADIO_TUNER_TUNER_PINC_OFFSET 1
#define RADIO_TUNER_CONTROL_REG_OFFSET 2
#define RADIO_TUNER_TIMER_REG_OFFSET 3
#define RADIO_PERIPH_ADDRESS 0x43c00000

#define FIFO_ADDRESS 0x43c10000
#define FIFO_RXD_DATA_OFFSET 8
#define FIFO_RXD_OCCUPANCY_OFFSET 7

// the below code uses a device called /dev/mem to get a pointer to a physical
// address.  We will use this pointer to read/write the custom peripheral
volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{

	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *base_addr = (volatile unsigned int *)map_base; 
	return (base_addr);
}

void radioTuner_tuneRadio(volatile unsigned int *ptrToRadio, float tune_frequency)
{
	float pinc = (-1.0*tune_frequency)*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_TUNER_PINC_OFFSET)=(int)pinc;
}

void radioTuner_setAdcFreq(volatile unsigned int* ptrToRadio, float freq)
{
	float pinc = freq*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_FAKE_ADC_PINC_OFFSET) = (int)pinc;
}

void play_tune(volatile unsigned int *ptrToRadio, float base_frequency)
{
	int i;
	float freqs[1] = {1760.0};
	float durations[1] = {1};
	for (i=0;i<1;i++)
	{
		radioTuner_setAdcFreq(ptrToRadio,freqs[i]+base_frequency);
		usleep((int)(durations[i]*500000));
	}
}

void read_and_print_FIFO(volatile unsigned int *fifo_base, volatile unsigned int *periph_base)
{
	unsigned int sample;
	int it = 0;
	unsigned int start_time;
    unsigned int stop_time;
	start_time = *(periph_base+RADIO_TUNER_TIMER_REG_OFFSET);
	while (it<480000)
	{
		if (*(fifo_base+FIFO_RXD_OCCUPANCY_OFFSET)>0)
		{
			sample = *(fifo_base+FIFO_RXD_DATA_OFFSET);
			it++;
		}
	}
	stop_time = *(periph_base+RADIO_TUNER_TIMER_REG_OFFSET);
	float time_spent = (stop_time-start_time)/125000000.0f;
	float throughput = 480000*4/(1000*time_spent);
	printf("480,000 samples read in %f seconds!\n\r", time_spent);
	printf("Data read rate: %f KiloBytes/second.\n\r", throughput);
}

int main()
{

	// first, get a pointer to the peripheral base address using /dev/mem and the function mmap
    volatile unsigned int *my_periph = get_a_pointer(RADIO_PERIPH_ADDRESS);	
	// and FIFO
	volatile unsigned int *my_fifo = get_a_pointer(FIFO_ADDRESS);	

    printf("\r\n\r\n\r\nLab 11 Ray Corriher - FIFO Read Demo\n\r");
    *(my_periph+RADIO_TUNER_CONTROL_REG_OFFSET) = 0; // make sure radio isn't in reset
    printf("Tuning Radio to 30MHz\n\r");
    radioTuner_tuneRadio(my_periph,0);
	printf("Playing Tune at near 3kHz\r\n");
    radioTuner_setAdcFreq(my_periph,3e3);

	printf("Occupancy: %x\n\r",*(my_fifo+FIFO_RXD_OCCUPANCY_OFFSET));
	printf("Data: %x\n\r",*(my_fifo+FIFO_RXD_DATA_OFFSET));
    read_and_print_FIFO(my_fifo, my_periph);
	
	radioTuner_setAdcFreq(my_periph,0);
    return 0;
}
