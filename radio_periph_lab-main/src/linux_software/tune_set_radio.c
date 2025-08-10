#include "get_a_pointer.h"
#include <stdlib.h>

#define _BSD_SOURCE

#define RADIO_PERIPH_ADDRESS 0x43c00000
#define RADIO_TUNER_FAKE_ADC_PINC_OFFSET 0
#define RADIO_TUNER_TUNER_PINC_OFFSET 1
#define RADIO_TUNER_CONTROL_REG_OFFSET 2
#define RADIO_TUNER_TIMER_REG_OFFSET 3

void radioTuner_tuneRadio(volatile unsigned int *ptrToRadio, int tune_frequency)
{
	float pinc = (-1.0*tune_frequency)*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_TUNER_PINC_OFFSET)=(int)pinc;
}

void radioTuner_setAdcFreq(volatile unsigned int* ptrToRadio, int freq)
{
	float pinc = freq*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_FAKE_ADC_PINC_OFFSET) = (int)pinc;
}

void radioTuner_activate(volatile unsigned int* ptrToRadio)
{
	*(ptrToRadio+RADIO_TUNER_CONTROL_REG_OFFSET) = 0x0002;
}

void radioTuner_deactivate(volatile unsigned int* ptrToRadio)
{
	*(ptrToRadio+RADIO_TUNER_CONTROL_REG_OFFSET) = 0x0000;
}

int main(int argc, char *argv[])
{
	// check correct inputs
	if (argc != 4) {
		fprintf(stderr, "Usage: %s [tuner freq] [ADC freq] [streaming? 0/1]\n", argv[0]);
		return EXIT_FAILURE;
    }
	// get frequencies
	int tuner_freq = atoi(argv[1]), adc_freq = atoi(argv[2]), stream_on = atoi(argv[3]);
	// get a pointer to the radio and fifo base address
    volatile unsigned int *my_radio = get_a_pointer(RADIO_PERIPH_ADDRESS);
	// set tuner and adc frequencies
	radioTuner_tuneRadio(my_radio,tuner_freq);
	radioTuner_setAdcFreq(my_radio,adc_freq);
	if (stream_on){radioTuner_activate(my_radio);}
	else{radioTuner_deactivate(my_radio);}
	return 0;
}