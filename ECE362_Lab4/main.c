#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#define  FAIL 0
#define  PASS 1

uint32_t fibonacci(uint32_t x);
void init_systick();
void init_rtsr();
void init_imr();
void init_iser();
void test_fibonacci();
void test_wiring();
void display_digit(int digit);
void display_digit_full(int digit);
void display_dot(int digit);
void init_systick();
void test_EXTI_1();
void test_EXTI_2();
void test_EXTI_3();
void test_EXTI_4();
unsigned int random (void);
void nano_wait(int);
void micro_wait(int);

// A count of the SysTick events.
volatile int tick_count;

//==========================================================
// Try the student's fibonacci() function.
//
void try_fibonacci(void) {
    // If student was unable to write fibonacci, replace the
    // line below with:
    //   micro_wait(2500000);
    unsigned int __attribute((unused)) x = fibonacci(27);
}

//==========================================================
// Run try_fibonacci() and blink the decimal point
// after each call.
//
void forever_fibonacci(void) {
    for(;;) {
        display_dot(0);
        try_fibonacci();
        display_dot(1);
        try_fibonacci();
    }
}

// Enable GPIOC clock
void enable_ports() {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
}

// Set the moder register so that pins PC0,1,2,3,8,9 are outputs
void portc_output() {
    GPIOC->MODER |= GPIO_MODER_MODER8_0;
    GPIOC->MODER |= GPIO_MODER_MODER9_0;
    GPIOC->MODER |= GPIO_MODER_MODER0_0;
    GPIOC->MODER |= GPIO_MODER_MODER1_0;
    GPIOC->MODER |= GPIO_MODER_MODER2_0;
}

// Test sys_tick
void test_SysTick(void) {
    enable_ports();
    portc_output();
    display_digit(0);
    init_systick();
    forever_fibonacci();
}

// A simple reaction timer
void reaction_timer() {
    enable_ports();
    portc_output();
    display_digit_full(0);
    init_rtsr();
    init_imr();
    init_iser();
    init_systick();
    nano_wait(10000 * random());
    GPIOC->ODR |= GPIO_ODR_8;
    while(1) {
        display_digit_full(tick_count);
    }
}

int main(void)
{
    //test_fibonacci();
    //test_wiring();
    //test_SysTick();
    //test_EXTI_1();
    //test_EXTI_2();
    //test_EXTI_3();
    //test_EXTI_4();
    reaction_timer();
}
