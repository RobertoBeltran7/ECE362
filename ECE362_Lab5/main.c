#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

void micro_wait(int);

void prob1(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODER9) |
	GPIO_MODER_MODER9_0;

	for(;;) {
		GPIOC->BSRR = GPIO_ODR_9;
		micro_wait(1000000);
		GPIOC->BRR = GPIO_ODR_9;
		micro_wait(1000000);
	}
}

void prob2(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	// Set the mode for PC8 for "alternate function"
	GPIOC->MODER &= ~(3<<18);
	GPIOC->MODER |= 2<<18;
	
	// Set the alternate function for PC8
	// PC0-7 are on AFR[0], 8-15 are on AFR[1]
	GPIOC->AFR[1] &= ~0xf;
	GPIOC->AFR[1] |= 1;
	
	// Enable the system clock for timer 3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	// Counting direction: 0=up, 1=down
	TIM3->CR1 &= ~TIM_CR1_DIR; // clear it to count up
	
	// Set prescaler output to 4kHz (48MHz/12000)
	TIM3->PSC = 12000 - 1;
	
	// Auto-reload 4000
	TIM3->ARR = 4000 - 1;
	
	// Any value between 0 and 4000.
	TIM3->CCR3 = 3456;
	
	// Channel 3 of the timer is configured in CCMR2.
	// Set the bits to select toggle mode (011)
	TIM3->CCMR2 &= ~TIM_CCMR2_OC3M_2; // Turn off bit 2.
	TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0;
	TIM3->CCMR2 &= ~TIM_CCMR2_OC4M_2; // Turn off bit 2.
	TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0;
	
	// Enable output for channel 3 active-high output
	TIM3->CCER |= TIM_CCER_CC3E;
	TIM3->CCER |= TIM_CCER_CC4E;
	
	// Enable timer 3
	TIM3->CR1 |= TIM_CR1_CEN;
	
	while(1)
		asm("wfi");
}

void prob3(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	// Set the mode for PC8 for "alternate function"
	GPIOC->MODER &= ~(3<<16);
	GPIOC->MODER |= 2<<16;

	GPIOC->MODER &= ~(3<<18);
	GPIOC->MODER |= 2<<18;
	
	// Set the alternate function for PC8
	// PC0-7 are on AFR[0], 8-15 are on AFR[1]
	GPIOC->AFR[1] &= ~0xf;
	GPIOC->AFR[1] |= 1;
	
	// Enable the system clock for timer 3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	// Counting direction: 0=up, 1=down
	TIM3->CR1 &= ~TIM_CR1_DIR; // clear it to count up
	
	// Set prescaler output to 4kHz (48MHz/12000)
	TIM3->PSC = 24000 - 1;
	
	// Auto-reload 4000
	TIM3->ARR = 2000 - 1;
	
	// Any value between 0 and 4000.
	TIM3->CCR3 = 1000;
	TIM3->CCR4 = 1000;
	
	// Channel 3 of the timer is configured in CCMR2.
	// Set the bits to select toggle mode (011)
	TIM3->CCMR2 &= ~TIM_CCMR2_OC3M_2; // Turn off bit 2.
	TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0;
	TIM3->CCMR2 &= ~TIM_CCMR2_OC4M_2; // Turn off bit 2.
	TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0;
	
	// Enable output for channel 3 active-high output
	TIM3->CCER |= TIM_CCER_CC3E;
	TIM3->CCER |= TIM_CCER_CC4E;
	
	// Enable timer 3
	TIM3->CR1 |= TIM_CR1_CEN;
	
	while(1)
		asm("wfi");
}

void prob4(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	// Set the mode for PC8 for "alternate function"
	GPIOC->MODER &= ~(3<<16);
	GPIOC->MODER |= 2<<16;

	GPIOC->MODER &= ~(3<<18);
	GPIOC->MODER |= 2<<18;
	
	// Set the alternate function for PC8
	// PC0-7 are on AFR[0], 8-15 are on AFR[1]
	GPIOC->AFR[1] &= ~0xf;
	GPIOC->AFR[1] |= 1;
	
	// Enable the system clock for timer 3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	// Counting direction: 0=up, 1=down
	TIM3->CR1 &= ~TIM_CR1_DIR; // clear it to count up
	
	// Set prescaler output to 4kHz (48MHz/12000)
	TIM3->PSC = 24000 - 1;
	
	// Auto-reload 4000
	TIM3->ARR = 2000 - 1;
	
	// Any value between 0 and 4000.
	TIM3->CCR3 = 500;
	TIM3->CCR4 = 1000;
	
	// Channel 3 of the timer is configured in CCMR2.
	// Set the bits to select toggle mode (011)
	TIM3->CCMR2 &= ~TIM_CCMR2_OC3M_2; // Turn off bit 2.
	TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0;
	TIM3->CCMR2 &= ~TIM_CCMR2_OC4M_2; // Turn off bit 2.
	TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0;
	
	// Enable output for channel 3 active-high output
	TIM3->CCER |= TIM_CCER_CC3E;
	TIM3->CCER |= TIM_CCER_CC4E;
	
	// Enable timer 3
	TIM3->CR1 |= TIM_CR1_CEN;
	
	while(1)
		asm("wfi");
}

void display(int x) {
    x &= 0xf; // Only look at the low 4 bits.

    //    _____
    //   |  A  |
    //  F|     |B
    //   |_____|
    //   |  G  |
    //  E|     |C
    //   |_____|
    //      D
    static const char output[16] = {
            0x40, // 0: _FEDCBA 1000000
            0x79, // 1: ____CB_ 1111001
            // complete the rest of the table...
            
            0x24,// 2: G_ED_BA
            0x30, // 3: G__DCBA
            0x19, // 4: GF__CB_
            0x12, // 5: GF_DC_A
            0x02, // 6: GFEDC_A
            0x78, // 7: ____CBA
            0x00, // 8: GFEDCBA
            0x18, // 9: GF__CBA
            0x08, // a: GFE_CBA
            0x03, // b: GFEDC__
            0x46, // c: _FED__A
            0x21, // d: G_EDCB_
            0x06, // e: GFED__A
            0x0E // f: GFE___A
    };
    GPIOC->ODR = output[x];

    // You need to look up the value of output[x] here
    // and then somehow assign it to the ODR.
}

void init_display(void) {
     // Enable Port C
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	GPIOC->MODER |= 0x5555;
	GPIOC->ODR =   0xff;
}

void test_display(void) {
    init_display();
   
    int x;
    for(;;)
        for(x=0; x<16; x++) {
            display(x);
            micro_wait(500000);
        }
}

int count = 0;
void increment(void) {
    count += 1;
    display(count);
}


void TIM2_IRQHandler(){
	increment();

	int c;
	c = TIM2->CCR1;
	c = TIM2->CCR2;
}

void prob6(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 

	init_display();
	display(0);
	GPIOA->MODER &= ~(3<<(2*0)); //Set up PA0
	GPIOA->MODER |= 2<<(2*0);
	GPIOA->AFR[0] &= ~(0xf<<(4*0));  //Set up AFR
	GPIOA->AFR[0] |=  0x2<<(4*0);
	RCC->APB1ENR |=RCC_APB1ENR_TIM2EN;  //Enable the system clock for timer 2
	TIM2->PSC = 1 - 1;					//Set prescaler output to maximum rate.
	TIM2->ARR = 0xffffffff;				//Auto-reload the maximum value

	TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;		//Set direction of channel 1 as input, and set TI1 as input
	TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;	//01 = input, CC1 mapped to TI1

	TIM2->CCER |= TIM_CCER_CC1E;		//Enable capture for CC1
	TIM2->DIER |= TIM_DIER_CC1IE;		//Allow Chan 1 to generate interrupts
	TIM2->CR1 |= TIM_CR1_CEN;			//Enable the timer counter
	NVIC->ISER[0] = 1<<TIM2_IRQn;		//Enable timer 2 interrupt in the interrupt controller (NVIC)

	for(;;)
		asm("wfi");

}

void prob7(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 

	init_display();
	display(0);
	GPIOA->MODER &= ~(3<<(2*1)); //Set up PA1
	GPIOA->MODER |= 2<<(2*1);
	GPIOA->AFR[0] &= ~(0xf<<(4*1));  //Set up AFR
	GPIOA->AFR[0] |=  0x2<<(4*1);
	GPIOA->PUPDR |= 0x8;

	RCC->APB1ENR |=RCC_APB1ENR_TIM2EN;  //Enable the system clock for timer 2
	TIM2->PSC = 1 - 1;					//Set prescaler output to maximum rate.
	TIM2->ARR = 0xffffffff;				//Auto-reload the maximum value

	TIM2->CCMR2 &= ~TIM_CCMR1_CC2S;		//Set direction of channel 1 as input, and set TI1 as input
	TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;	//01 = input, CC1 mapped to TI1

	TIM2->CCMR1 |= TIM_CCMR1_IC2F_3 | TIM_CCMR1_IC2F_2 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC2F_0;


	TIM2->CCER |= TIM_CCER_CC2E;		//Enable capture for CC2
	TIM2->DIER |= TIM_DIER_CC2IE;		//Allow Chan 2 to generate interrupts
	TIM2->CR1 |= TIM_CR1_CEN;			//Enable the timer counter

	TIM2->CR1 |= 0x200;

	NVIC->ISER[0] = 1<<TIM2_IRQn;		//Enable timer 2 interrupt in the interrupt controller

	for(;;)
			asm("wfi");

}

void TIM3_IRQHandler(){
	GPIOC->ODR ^=  0x80;
	TIM3->SR &= ~0x1;
}

void prob8(void) {

	RCC->APB1ENR |=RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 47999;
	TIM3->ARR = 499;

	TIM3->DIER |= 1;
	NVIC->ISER[0] = 1<<TIM3_IRQn;
	TIM3->CR1 |= TIM_CR1_CEN;			//Enable the timer counter

	prob7();

}

int main(void) {
    //prob1();
    //prob2();
    prob3();
    //prob4();
    //test_display();
    //prob6();
    //prob7();
    //prob8();
}
