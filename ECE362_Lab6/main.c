#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "wavetable.h"
#include <stdio.h>
#include <stdlib.h>
#define SAMPLES 128

volatile int index = 0;
volatile int index1 = 0;

void micro_wait(unsigned int micro_seconds);

int s1 = 0;
int s2 = 0;
float a1 = 1;
float a2 = 1;
int circ_buf[SAMPLES] = {0};
uint32_t start = 0, end = 0;
			
void insert_circ_buf(int val) {
    if(start <= end && end <= (SAMPLES-1)) {
        circ_buf[end++] = val;
    } else if(start < end && end > (SAMPLES-1)) {
        circ_buf[0] = val;
        end = 0;
        start = end + 1;
    } else if(end < start && start < (SAMPLES-1)) {
        start++;
        circ_buf[end++] = val;
    } else {
        start = 0;
        circ_buf[end++] = val;
    }
}

float get_time_period(int min, int max) {
    int start_interval = 0;
    int sample_count = 0;
    int avg_samples = 0;
    int no_cycles = 0;

    for(int i = 0; i < SAMPLES; i++) {


        if(circ_buf[i] < (0.3*max) && start_interval == 1) {
            start_interval = 0;
            avg_samples += sample_count;
            sample_count = 0;
            no_cycles++;
        }

        if(circ_buf[i] > (0.8*max) && start_interval == 0) {
            start_interval = 1;

        }


        if(start_interval == 1)
            sample_count++;
    }

    int avg_interval = avg_samples / no_cycles;
    return (2 * avg_interval * 200e-6);
}

// This function
// 1) enables clock to port A,
// 2) sets PA0, PA1, PA2 and PA4 to analog mode
void setup_gpio() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= 0x33F;
}

// This function should enable the clock to the
// onboard DAC, enable trigger,
// setup software trigger and finally enable the DAC.
void setup_dac() {
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	DAC->CR &= ~DAC_CR_EN1;
	DAC->CR &= ~DAC_CR_BOFF1;
	DAC->CR |= DAC_CR_TEN1;
	DAC->CR |= DAC_CR_TSEL1;
	DAC->CR |= DAC_CR_EN1;

}

// This function should,
// enable clock to timer2,
// setup prescalaer and arr so that the interrupt is triggered 100us,
// enable the timer 2 interrupt and start the timer.
void setup_timer2() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->CR1 &= TIM_CR1_CEN;
	TIM2->PSC = 480 - 1;
	TIM2->ARR = 10 - 1;
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC->ISER[0] = 1<<TIM2_IRQn;
	TIM2->CR1 |= TIM_CR1_CEN;

}

// This function should, enable clock to timer3,
// setup prescalaer and arr so that the interrupt is triggered 200us,
// enable the timer 3 interrupt and start the timer.
void setup_timer3() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CR1 &= TIM_CR1_CEN;
	TIM3->PSC = 960 - 1;
	TIM3->ARR = 10 - 1;
	TIM3->DIER |= TIM_DIER_UIE;
	NVIC->ISER[0] = 1<<TIM3_IRQn;
	TIM3->CR1 |= TIM_CR1_CEN;
}

// This function should enable the clock to ADC,
// turn on the clocks, wait for ADC to be ready.
void setup_adc() {
	//RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->CR2 |= RCC_CR2_HSI14ON;
	while(!(RCC->CR2 & RCC_CR2_HSI14RDY));
	ADC1->CR |= ADC_CR_ADEN;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
	while((ADC1->CR & ADC_CR_ADSTART));
}

// This function should return the value from the ADC
// conversion of the channel specified by the “channel” variable.
// Make sure to set the right bit in channel selection
// register and do not forget to start adc conversion.
int read_adc_channel(unsigned int channel) {
		ADC1->CHSELR = 0;
		ADC1->CHSELR |= 1 << channel;
		while(!(ADC1->ISR & ADC_ISR_ADRDY));
		ADC1->CR |=ADC_CR_ADSTART;
		while(!(ADC1->ISR & ADC_ISR_EOC));
		return (int)(ADC1->DR);
}

//The interrupt handler should read the value from the ADC’s channel 2 input.
// Use the insert_circ_buffer() function to insert the read value into the circular buffer.
void TIM3_IRQHandler() {
	TIM3->SR &= ~TIM_SR_UIF;
	int val = read_adc_channel(2);
	insert_circ_buf(val);

}

// TIM2_IRQHandler: The interrupt handler should start the DAC conversion using the software trigger,
// and should use the wavetable.h to read from the array and write it into the DAC.
// Every time the interrupt is called you will read a new element from the “wavetable” array.
// So you might need to use a global variable as an index to the array.
// Note that the array has 100 elements, make sure you do not read wavetable[100].
void TIM2_IRQHandler() {

	while((DAC->SWTRIGR & DAC_SWTRIGR_SWTRIG1) == DAC_SWTRIGR_SWTRIG1);

	DAC->DHR12R1 = (int)(a1 * wavetable[index] + a2 * wavetable[index1]) >>1;

	DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
	index++;
	index1 +=2;
	if (index == 100)
		index = 0;
	if(index1 == 100)
		index1 = 0;
	TIM2->SR &= ~TIM_SR_UIF;
}

int main(void)
{
    serial_init();
    setup_gpio();
    setup_adc();
    setup_dac();
    setup_timer2();
    setup_timer3();

    while(1) {
    	a1 = read_adc_channel(0) / 4095.0;
    	a2 = read_adc_channel(1) / 4095.0;
    	serial_init();
    	int max = circ_buf[0];
    	int min = circ_buf[0];
    	for(int i = 0; i < SAMPLES; i++){
    		int num = circ_buf[i];
    		if(num < min)
    			min = num;
    		if(num > max)
    			max = num;
    	}
    	float period = get_time_period(min, max);

    	float frequency = 1.0 / period;
    	printf("a1 is %d\n", (int)(a1 * 1000));
    	printf("a2 is %d\n", (int)(a2 * 1000));
    	printf("frequency is %d\n", (int)(frequency * 1000));
        micro_wait(500000);
    }
}
