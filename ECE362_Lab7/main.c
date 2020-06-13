#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdlib.h>

#define COL1 (1 << 0)
#define COL2 (1 << 1)
#define COL3 (1 << 2)
#define COL4 (1 << 3)

#define ROW1 (1 << 4)
#define ROW2 (1 << 5)
#define ROW3 (1 << 6)
#define ROW4 (1 << 7)

#define SAMPLE_TIME_MS 10
#define SAMPLE_COUNT (SAMPLE_TIME_MS)

#define THRESHOLD_TIME_MS 2
#define THRESHOLD (THRESHOLD_TIME_MS)

#define KEY_PRESS_MASK  0b11000111
#define KEY_REL_MASK    0b11100011

void init_lcd(void);
void display1(const char *);
void display2(const char *);

int col = 0;
int row = -1;
int red = 0, blue = 0, grn = 0;
char char_array[4][4] = { {'1', '2', '3', 'A'},
                          {'4', '5', '6', 'B'},
                          {'7', '8', '9', 'C'},
                          {'*', '0', '#', 'D'} };


int int_array[4][4] =   { {1,2,3,0xa},
                          {4,5,6,0xb},
                          {7,8,9,0xc},
                          {0xf, 0, 0xe, 0xd} };

uint8_t key_samples[4][4]  = { {0}, {0}, {0}, {0} };
uint8_t key_pressed[4][4]  = { {0}, {0}, {0}, {0} };
uint8_t key_released[4][4]  = { {0}, {0}, {0}, {0} };

void update_key_press() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if ((key_samples[i][j] & KEY_PRESS_MASK) == 0b00000111) {
                key_pressed[i][j] = 1;
                key_samples[i][j] = 0xFF;
            }

            if ((key_samples[i][j] & KEY_REL_MASK) == 0b11100000) {
                key_released[i][j] = 1;
                key_samples[i][j] = 0x00;
            }
        }
    }
}

char get_char_key() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(key_released[i][j] == 1 && key_pressed[i][j] == 1) {
                key_released[i][j] = 0;
                key_pressed[i][j] = 0;
                return char_array[i][j];
            }
        }
    }

    return '\0';
}

int get_key_pressed() {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            if(key_released[i][j] == 1 && key_pressed[i][j] == 1) {
                key_released[i][j] = 0;
                key_pressed[i][j] = 0;
                return int_array[i][j];
            }
        }
    }

    return -1;
}

void update_samples(int row) {
    // Update the current column with new values
    for(int i = 0; i < 4; i++) {
        uint8_t curr_history = key_samples[i][col];
        curr_history = curr_history << 1;

        if(row == i)
            curr_history |= 1;

        key_samples[i][col] = curr_history;
    }
}

int get_user_freq() {
    char line1[16] = "Freq: ______";
    int freq = 0;
    int pos = 0;
    display1(line1);
    while(1) {
        int key = get_key_pressed();
        if(key == 0x0d)
            break;
        if(key >= 0 && key <= 9) {
            freq = freq * 10 + key;
            line1[6+pos] = key + '0';
            display1(line1);
            pos++;
            if (pos == 6)
                break;
        }
    }

    return freq;
}

void get_pwm_duty() {
    char line2[16] = "Duty: ______";
    int pos = 0;
    display2(line2);
    red = 0;
    grn = 0;
    blue = 0;
    while(1) {
        int key = get_key_pressed();
        if (key >= 0 && key <= 9) {
            switch(pos) {
            case 0: red = 10 * key;
            break;
            case 1: red = red + key;
            break;
            case 2: grn = 10 * key;
            break;
            case 3: grn = grn + key;
            break;
            case 4: blue = 10 * key;
            break;
            case 5: blue = blue + key;
            break;
            }
            line2[6+pos] = key + '0';
            display2(line2);
            pos++;
            if (pos == 6)
                break;
        }

    }
}

void prob2() {
    setup_gpio();
    setup_pwm();
    update_freq(1000);
    int r, g, b;
    r = g = b = 0;
    int state = 0;
    while(1) {
        if(r == 100) {
            state = 1;
        } if(g == 100) {
            state = 2;
        } if(b == 100) {
            state = 3;
        } if(r == 100) {
            state = 1;
        }

        if(state == 0) {
            r = r + 1;
        }
        if(state == 1) {
            r = r - 1;
            g = g + 1;
        }

        if(state == 2) {
            g = g - 1;
            b = b + 1;
        }

        if(state == 3) {
            r = r + 1;
            b = b - 1;
        }

        update_rgb(r, g, b);
        nano_wait(10000000);
    }
}

void prob3(void) {
    char keys[16] = {"Key Pressed:"};
    init_lcd();
    init_keypad();
    setup_timer3();
    display1("Problem 3");
    display2(keys);
    while(1) {
        char key = get_char_key();
        if(key != '\0') {
            keys[12] = key;
            display2(keys);
        }
    }
}

void prob4(void) {
    init_lcd();
    init_keypad();
    setup_gpio();
    setup_pwm();
    setup_timer3();
    display1("Freq: 001000");
    display2("Duty: 999999");

    while(1) {
    	int key_pressed = get_key_pressed();
    	if(key_pressed == 0xf){
    		int freq = get_user_freq();
    		update_freq(freq);
    	}
    	if(key_pressed == 0xe){
    		get_pwm_duty();
    		update_rgb(red, grn, blue);
    	}

    }
}

// This function should enable the clock to port A, configure pins 0, 1, 2 and
// 3 as outputs (we will use these to drive the columns of the keypad).
// Configure pins 4, 5, 6 and 7 to have a pull down resistor
// (these four pins connected to the rows will being scanned
// to determine the row of a button press).
void init_keypad() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= 0xFFFF0000;
	GPIOA->MODER |= 0x55;
	GPIOA->PUPDR &= 0xFFFF00FF;
	GPIOA->PUPDR |= 0xAA00;

}

// This function should enable clock to timer 3, setup the timer
// so that it triggers TIM3_IRQHandler every 1ms.
void setup_timer3() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 480 - 1;
	TIM3->ARR = 100 - 1;
	TIM3->DIER |= TIM_DIER_UIE;
	NVIC->ISER[0] = 1<<TIM3_IRQn;
	TIM3->CR1 |= TIM_CR1_CEN;

}

// Use the global variable ‘col’ provided in the skeleton file,
// increment it by 1. Check if ‘col’ exceeds 3, if so, reset it to 0.
// This is because we have 4 columns on the keypad and ‘col’ dictates which
// column is currently being driven. Set the output data register
// of the port A to (1 << col). This drives the corresponding column to ‘logic 1’.
void TIM3_IRQHandler()
{

	int current_row = -1;
	int x = GPIOA->IDR & 0xF0;
	if(x & ROW1)
		current_row = 0;
	else if(x & ROW2)
		current_row = 1;
	else if(x & ROW3)
		current_row = 2;
	else if(x & ROW4)
		current_row = 3;
	update_samples(current_row);
	update_key_press();
	col +=1;
	if(col > 3)
		col = 0;
	GPIOA->ODR = 1 << col;
	TIM3->SR &= ~TIM_SR_UIF;
}

// Should enable clock to GPIO port A, configure the modes of the three
// pins corresponding to TIM1_CH1, TIM1_CH2 and TIM1_CH3 as alternate function.
void setup_gpio() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= 0xFFC0FFFF;
	GPIOA->MODER  |= 0x2A0000;
	GPIOA->AFR[1] &= 0xFFFFF000;
	GPIOA->AFR[1] |= 0x222;
}

// Should use TIM1 to PSC so that the clock is 100 kHz, and choose the
// value of ARR so that the PWM frequency is 1 kHz. The duty cycle of each
// channel will be set by writing a value between 0 and 100 to the CCRx
// registers. Note since we have a common anode configuration,
// CCRx of 100 will result in an off LED and
// a CCRx of 0 will result in maximum brightness.
void setup_pwm() {
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	TIM1->PSC = 480 - 1;
	TIM1->ARR = 100-1;
	TIM1->CCR1 = 50;
	TIM1->CCR2 = 50;
	TIM1->CCR3 = 50;
	TIM1->CCR4 = 50;
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4PE;

	TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	TIM1->BDTR |= TIM_BDTR_MOE;
	TIM1->CR1 |= TIM_CR1_CEN;
	TIM1->EGR |= TIM_EGR_UG;

}

// This function accepts an integer argument that is used to update the
// TIM1_PSC to produce the requested frequency (as close as possible) on
// the output pins. Remember that the output frequency will be 100 times
// slower than the TIM1_PSC due to TIM1_ARR always being 100-1 (99).
// The formula for calculating the output frequency is then:
//          freq = 48,000,000.0 / (TIM1_PSC + 1) / 100.0
// You should determine the formula to use to put the proper value
// into TIM1_PSC given the frequency
void update_freq(int freq) {
	float f = 0.0;
	f = 480000.0/freq - 1;
	TIM1->PSC = f;


}

// This function accepts three arguments---the red, green, and blue values used
// to set the CCRx registers. The values should never be smaller than zero or
// larger than 100. The value can be assigned directly to the appropriate
// CCR registers. E.g. the red LED is connected to channel 1.
void update_rgb(int r, int g, int b) {
	TIM1->CCR1 = 100 - r;
	TIM1->CCR2 = 100 - g;
	TIM1->CCR3 = 100 - b;
}

int main(void)
{
    //test_wiring();
    //prob2();
    //prob3();
    prob4();
}

