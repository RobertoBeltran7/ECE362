#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdio.h>
#include <stdint.h>
#include "font5x8.h"
#include "wavetable.h"                  // FOR SOUND
#define SAMPLES 128                     // FOR SOUND

#define DB0 (1<<0)
#define DB1 (1<<1)
#define DB2 (1<<2)
#define DB3 (1<<3)
#define DB4 (1<<4)
#define DB5 (1<<5)
#define DB6 (1<<6)
#define DB7 (1<<7)

#define Data_Mask DB0|DB1|DB2|DB3|DB4|DB5|DB6|DB7

#define CS2 (1<<8)
#define CS1 (1<<9)
#define RW  (1<<10)                 // now tied to ground
#define DI  (1<<11)
#define EN  (1<<12)

#define ball_radius 2
#define left_paddle_x 2 + 15               // x coordinate of center of left paddle
#define right_paddle_x 125 - 15            // x coordinate of center of right paddle

void Initialize_LCD(void);
void LCD_cmd(unsigned char, unsigned char);
void LCD_Clear(void);
void clearScreen(void);
void LCD_coord(unsigned char, unsigned char);
void LCD_WriteData(unsigned char, unsigned char);
//void LCD_WriteString(char *);
//void LCD_WriteChar(char);
//unsigned char Read_Byte(char *);
//void LCD_Delay(void);
void nano_wait(unsigned int);
void makeCircle(unsigned char ball_x, unsigned char ball_y);
void clearCircle(unsigned char ball_x, unsigned char ball_y);
void makePaddle(unsigned char paddle, unsigned char paddle_y);
void clearPaddle(unsigned char paddle, unsigned char paddle_y);
void set(unsigned char x, unsigned char y);
void clear(unsigned char x, unsigned char y);
int get_paddle_dy(int side);
void ADCinit(void);

//void (*cmd)(char a) = 0;
//void (*data)(char a) = 0;

// global variables
unsigned char disp[2][64][8] = {0};
int global_left_paddle_dy, global_right_paddle_dy;
unsigned char global_left_paddle_y = 32;
unsigned char global_right_paddle_y = 32;
unsigned char global_ball_x = 64;
unsigned char global_ball_y = 32;
unsigned char dx = 1;
unsigned char dy = -1;
unsigned char scoreleft;
unsigned char scoreright;

void nano_wait(unsigned int i)
{
    asm(    "       mov r0,%0\n"
            "loop:  sub r0,#83\n"
            "       bgt loop\n" : : "r"(i) : "r0", "cc");
}

// Toggles Pin 12
void enable(void)
{
    GPIOB->BSRR= EN;
    GPIOB->BRR = EN;
    nano_wait(100);
    GPIOB->BSRR = EN;
    nano_wait(100);
    GPIOB->BRR = EN;
}

// Initialize LCD Display
void Initialize_LCD(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~(0x3ffffff);
    GPIOB->MODER |= 0x1555555;      //Set pins 0-12 for output

    GPIOB->BRR = CS2 | CS1 | RW | DI | EN;  //Set all control ports low     -- RW now tied to ground
    nano_wait(1500);

    unsigned char i;
    for (i=0;i<2;i++)
    {
        LCD_cmd(0x3f,i);    //Turn on the display (0x3f turns on display)
    }

}

// Sets commands for the LCD Display
void LCD_cmd(unsigned char cmd, unsigned char chip_select) // always writes
{
    if (chip_select == 0){      //Select the chip
        GPIOB->BSRR = CS2;      // high
        GPIOB->BRR = CS1;       // low -- active
    }
    else
    {
        GPIOB->BSRR = CS1;      // high
        GPIOB->BRR = CS2;       // low -- active
    }

    GPIOB->BRR = DI | RW;       // both low for address commands        -- RW tied to ground

    GPIOB->BSRR = cmd | (0xff<<16);      //Sets the command in the ouptput register

    nano_wait(200);

    GPIOB->BSRR = EN;          //High Pulse
    nano_wait(4000);

    // ---------------- WRITING COMMAND NOW ---------------------- //

    GPIOB->BRR = EN;           //Low Pulse
    nano_wait(4000);

}

void LCD_Clear(void)
{
    unsigned char i,j;
    for (i=0;i<8;i++)
    {
        LCD_coord(0,i);
        for (j=0;j<128;j++)
        {
            LCD_WriteData(j, 0x00);
        }
    }
    LCD_coord(0,0);     //Go to initial position (Top,left corner)
}

void clearScreen()
{
    unsigned char x_cord, y_cord;
    for(y_cord=0; y_cord<64; y_cord++)
    {
        for (x_cord=0; x_cord<128; x_cord++)
        {
            if ((x_cord == 70) && (y_cord == 5))
            {
                asm("nop");
            }
            clear(x_cord, y_cord);
        }
    }
}

void LCD_coord(unsigned char x, unsigned char y)
{

    LCD_cmd(0x40 | (x%64), (x/64));
    LCD_cmd(0xB8 | (y/8), (x/64));
    LCD_cmd(0xC0 | 0, (x/64));    //Set the display line
}

void LCD_WriteData(unsigned char x, unsigned char data)
{
    if (x>63){                  //Select the chip (Left half of LCD)
        GPIOB->BSRR = CS1;      // high
        GPIOB->BRR = CS2;       // low
    }
    else                        //Select the chip (Right half of LCD)
    {
        GPIOB->BSRR = CS2;      // high
        GPIOB->BRR = CS1;       // low
    }


    GPIOB->BRR = RW;            // low for write -- now tied to ground
    GPIOB->BSRR = DI;           //high for data

    GPIOB->BSRR = data | (0xff<<16);      //Sets the command in the ouptput register

    nano_wait(200);

    GPIOB->BSRR = EN;           //High Pulse
    nano_wait(4000);

    // ---------------- WRITING DATA NOW ---------------------- //

    GPIOB->BRR = EN;           //Low Pulse
    nano_wait(4000);

}

// Set display cursor
void set(unsigned char x, unsigned char y)
{
    unsigned char data = 1 << (y%8);
    unsigned char screen;

    if (x > 63)
    {
        screen = 1;
    }
    else
    {
        screen = 0;
    }


    disp[screen][x%64][y/8] |= data;

    // place cursor
    LCD_cmd(0x40 | (x%64), screen);  //Set the x-address
    LCD_cmd(0xB8 | (y/8), screen);   //Set the y-address
    LCD_cmd(0xC0 | 0, screen);   //Set the display line

    // write data byte at cursor location
    LCD_WriteData(x, disp[screen][x%64][y/8]);

}

void clear(unsigned char x, unsigned char y)
{
    unsigned char data = 1 << (y%8);
    int screen;

    if (x > 63)
    {
        screen = 1;
    }
    else
    {
        screen = 0;
    }

    disp[screen][x%64][y/8] &= ~data;

    // place cursor
    LCD_cmd(0x40 | (x%64), screen);          //Set the x-address
    LCD_cmd(0xB8 | (y/8), screen);      //Set the y-address
    LCD_cmd(0xC0 | 0, screen);          //Set the display line

    LCD_WriteData(x, disp[screen][x%64][y/8]);

}

//Create a small circle/ball to be used for the pong game
void makeCircle(unsigned char ball_x, unsigned char ball_y)
{

int yy, xchange, ychange, radiusError, radius;
radius = ball_radius;

yy = 0;
xchange = 1 - 2 * ball_radius;
ychange = 1;
radiusError = 0;

while(radius >= yy)
  {
    set(ball_x + radius, ball_y + yy); // LCD_SetPix() = set()
    set(ball_x - radius, ball_y + yy);
    set(ball_x - radius, ball_y - yy);
    set(ball_x + radius, ball_y - yy);
    set(ball_x + yy, ball_y + radius);
    set(ball_x - yy, ball_y + radius);
    set(ball_x - yy, ball_y - radius);
    set(ball_x + yy, ball_y - radius);
  yy++;
  radiusError += ychange;
  ychange += 2;
  if ( 2*radiusError + xchange > 0 )
    {
    radius--;
    radiusError += xchange;
    xchange += 2;
    }
  }
}

// Clear circle/ball from display
void clearCircle(unsigned char ball_x, unsigned char ball_y)
{

int yy, xchange, ychange, radiusError, radius;
radius = ball_radius;

yy = 0;
xchange = 1 - 2 * ball_radius;
ychange = 1;
radiusError = 0;

while(radius >= yy)
  {
    clear(ball_x + radius, ball_y + yy); // LCD_SetPix() = set()
    clear(ball_x - radius, ball_y + yy);
    clear(ball_x - radius, ball_y - yy);
    clear(ball_x + radius, ball_y - yy);
    clear(ball_x + yy, ball_y + radius);
    clear(ball_x - yy, ball_y + radius);
    clear(ball_x - yy, ball_y - radius);
    clear(ball_x + yy, ball_y - radius);
  yy++;
  radiusError += ychange;
  ychange += 2;
  if ( 2*radiusError + xchange > 0 )
    {
    radius--;
    radiusError += xchange;
    xchange += 2;
    }
  }
}

// Create paddles to use for the pong game
void makePaddle(unsigned char paddle, unsigned char paddle_y){
    int paddle_x;             // x coordinate of center of paddle 3 or 125
    if (paddle == 0){
        paddle_x = left_paddle_x;         // 2
    }
    else paddle_x = right_paddle_x;       // 125

    int i, j;
    for(i=0;i<3;i++){
        for(j=0;j<15;j++){
            set(paddle_x - 1 + i, paddle_y - 7 + j);
        }
    }
}

void clearPaddle(unsigned char paddle, unsigned char paddle_y){
    int paddle_x;             // x coordinate of center of paddle 3 or 125
    if (paddle == 0){
        paddle_x = left_paddle_x;         // 2
    }
    else paddle_x = right_paddle_x;       // 125

    int i, j;
    for(i=0;i<3;i++){
        for(j=0;j<15;j++){
            clear(paddle_x - 1 + i, paddle_y - 7 + j);
        }
    }
}

void ADCinit(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= 0xf;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CR2 |= RCC_CR2_HSI14ON;
    while(!(RCC->CR2 & RCC_CR2_HSI14RDY));
    ADC1->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY));
    while((ADC1->CR & ADC_CR_ADSTART));
    
}

int get_paddle_dy(int side)
{
    // uses pa0, pa1
    //left = 0;
    //right = 1;
        ADCinit();
        ADC1->CHSELR = 0;
        ADC1->CHSELR |= 1<<side;
        while(!(ADC1->ISR & ADC_ISR_ADRDY));
        ADC1->CR |= ADC_CR_ADSTART;
        while(!(ADC1->ISR & ADC_ISR_EOC));

        int dy;
        if ((ADC1->DR > 100) && (ADC1->DR <= 1750))
        {
            // Medium Speed Move up or down (Depending on joystick direction)
            dy = 1;                 // moves down slow
        }
        else if(ADC1->DR < 100)
        {
            // Fast Speed up or down
            dy = 2;                 // moves down fast
        }

        else if ((ADC1->DR > 2150) && (ADC1->DR <= 3550))
        {
            // Medium Speed Move up or down (Depending on joystick direction & opposite of Medium speed above)
            dy = -1;                 // moves up slow
        }
        else if(ADC1->DR > 3550)
        {
            // Fast Speed up or down in opposite direction
            dy = -2;                 // moves up fast
        }
        else
        {
            dy = 0;                 // idle
        }

        return dy;
}

//void LCD_WriteString(unsigned char x, char * string)
//{
//    while(*string)
//        LCD_WriteChar(x, *string++);
//}
//
//void LCD_WriteChar(unsigned char x, char character)
//{
//    int i;
//    character -= 32;
//    for (i=0;i<5;i++)
//    {
//        LCD_WriteData(x, Read_Byte((char *)((int)font5x8 + (5*character) + i)));
//    }
//    LCD_WriteData(0x0);
//}
//
//unsigned char Read_Byte(char * ptr)
//{
//    return *(ptr);
//}

int main(void)
{
//    tetris();
    Initialize_LCD();
    clearScreen();
    
    makePaddle(0, global_left_paddle_y);    // Player on left side of display
    
    makePaddle(1, global_right_paddle_y);   // Player on right side of display
    
    makeCircle(global_ball_x, global_ball_y);

    ADCinit();
    for (;;)
    {
        global_left_paddle_dy = get_paddle_dy(0);               // pa0
        global_right_paddle_dy = get_paddle_dy(1);               // pa1

        clearPaddle(0, global_left_paddle_y);
        clearPaddle(1, global_right_paddle_y);

        if ((global_left_paddle_y < 10) && (global_left_paddle_dy < 0))             // no further up
        {
            global_left_paddle_dy = 0;
        }
        else if ((global_left_paddle_y > (63-10)) && (global_left_paddle_dy > 0))   // no further down
        {
            global_left_paddle_dy = 0;
        }

        if ((global_right_paddle_y < 10) && (global_right_paddle_dy < 0))             // no further up
        {
            global_right_paddle_dy = 0;
        }
        else if ((global_right_paddle_y > (63-10)) && (global_right_paddle_dy > 0))   // no further down
        {
            global_right_paddle_dy = 0;
        }

        global_left_paddle_y += global_left_paddle_dy;
        global_right_paddle_y += global_right_paddle_dy;

        makePaddle(0, global_left_paddle_y);
        makePaddle(1, global_right_paddle_y);

        clearCircle(global_ball_x,global_ball_y);

        //checks if collide with left paddle
        if(((global_ball_x - ball_radius) == (left_paddle_x + 1)) && (global_left_paddle_y - 10 <= global_ball_y) && (global_ball_y <= global_left_paddle_y + 10)){
//            dx = -dx;
            if((global_left_paddle_y - 10 <= global_ball_y) && (global_ball_y <= global_left_paddle_y - 7)){
                dy = -2;
                dx = 1;
            }
            else if((global_left_paddle_y - 6 <= global_ball_y) && (global_ball_y <= global_left_paddle_y - 1)){
                dy = -1;
                dx = 1;
            }
            else if(global_left_paddle_y == global_ball_y){
                dy = 0;
                dx = 1;
            }
            else if((global_left_paddle_y + 1 <= global_ball_y) && (global_ball_y <= global_left_paddle_y + 6)){
                dy = 1;
                dx = 1;
            }
            else if((global_left_paddle_y + 7 <= global_ball_y) && (global_ball_y <= global_left_paddle_y + 10)){
                dy = 2;
                dx = 1;
            }
        }

        //checks if collide with right paddle
        else if(((global_ball_x + ball_radius) == (right_paddle_x - 1)) && (global_right_paddle_y - 10 <= global_ball_y) && (global_ball_y <= global_right_paddle_y + 10)){
//            dx = -dx;
            if((global_right_paddle_y - 10 <= global_ball_y) && (global_ball_y <= global_right_paddle_y - 7)){
                dy = -2;
                dx = -1;
            }
            else if((global_right_paddle_y - 6 <= global_ball_y) && (global_ball_y <= global_right_paddle_y - 1)){
                dy = -1;
                dx = -1;
            }
            else if(global_right_paddle_y == global_ball_y){
                dy = 0;
                dx = -1;
            }
            else if((global_right_paddle_y + 1 <= global_ball_y) && (global_ball_y <= global_right_paddle_y + 6)){
                dy = 1;
                dx = -1;
            }
            else if((global_right_paddle_y + 7 <= global_ball_y) && (global_ball_y <= global_right_paddle_y + 10)){
                dy = 2;
                dx = -1;
            }
        }

        //checks if ball reaches either side of display, if so reset and turn around
        else if(global_ball_x - ball_radius <= 0 || global_ball_x + ball_radius >= 127){
            clearCircle(global_ball_x,global_ball_y);
            global_ball_x = 64;
            global_ball_y = 32;
            nano_wait(1000000000);
            dx = -dx;
        }

        //checks if ball reaches top/bottom of display, if so turn around
        else if(global_ball_y - ball_radius <= 0 || global_ball_y + ball_radius >=63)
            dy = -dy;

        global_ball_x += dx;
        global_ball_y += dy;

        makeCircle(global_ball_x, global_ball_y);

        nano_wait(30000000);                                   // 50 ms
    }

    return 0;
}

// ================================================================================================================================= //
// ================================================== CODE BELOW IS ALL FOR SOUND ================================================== //
// ================================================================================================================================= //
void micro_wait(unsigned int micro_seconds);

int s1 = 0;
int s2 = 0;
float a1 = 1;
float a2 = 1;
int circ_buf[SAMPLES] = {0};
uint32_t start = 0, end = 0;

// declare global variables
int global_counter1 = 0;
int global_counter2 = 0;
//float a1, a2;

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
    GPIOA->MODER |= (3<<2*0) | (3<<2*1) | (3<<2*2) | (3<<2*4);
}

// This function should enable the clock to the
// onboard DAC, enable trigger,
// setup software trigger and finally enable the DAC.
void setup_dac() {
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;      // Enable clock to DAC
    DAC->CR &= ~DAC_CR_EN1;                 // Disable DAC channel 1
    DAC->CR &= DAC_CR_BOFF1;                // Do not turn buffer off.
    DAC->CR |= DAC_CR_TEN1;                 // Enable trigger
    DAC->CR |= DAC_CR_TSEL1;                // All ones.  Select software trigger.
    DAC->CR |= DAC_CR_EN1;                  // Enable DAC channel 1
}

// This function should,
// enable clock to timer2,
// setup prescalaer and arr so that the interrupt is triggered 100us,
// enable the timer 2 interrupt and start the timer.
void setup_timer2() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;     // enable clock to timer2

    TIM2->CR1 &= ~TIM_CR1_CEN;              // disable this before making changes

    // 1/100us = 10 kHz,
    // Set prescaler output to 10 kHz (48 MHz / 4800)
//    TIM2->PSC = 2400 - 1;
    TIM2->PSC = 1200 - 1;
    // Auto-reload 10000
    TIM2->ARR = 2 - 1;

    TIM2->DIER |= TIM_DIER_UIE;                 // Update interrupt enabled
    TIM2->CR1 |= TIM_CR1_CEN;               // Enable the timer counter
    NVIC->ISER[0] = 1<<TIM2_IRQn;           // Enable timer 2 interrupt in the interrupt controller (NVIC)

    //TIM2->CR1 |= TIM_CR1_CEN;               // enable the timer 2 interrupt

    //NVIC->ISER[0] = 1<<TIM2_IRQn;

}


// TIM2_IRQHandler: The interrupt handler should start the DAC conversion using the software trigger,
// and should use the wavetable.h to read from the array and write it into the DAC.
// Every time the interrupt is called you will read a new element from the wavetable array.
// So you might need to use a global variable as an index to the array.
// Note that the array has 100 elements, make sure you do not read wavetable[100].
void TIM2_IRQHandler() {
    //DAC->CR |= DAC_CR_TSEL1;        // start DAC conversion using software trigger
    TIM2 -> SR &= ~TIM_SR_UIF;
    // Wait for DAC to clear the SWTRIG1 bit
    while((DAC->SWTRIGR & DAC_SWTRIGR_SWTRIG1) == DAC_SWTRIGR_SWTRIG1);
    // Put new value into 12-bit, right-aligned holding register.

    // 6.2
    if (global_counter1 == 100)
        {
           global_counter1 = 0;
        }
    //DAC->DHR12R1 = wavetable[global_counter1++];

    // 6.3
    if (global_counter2 == 100)
        {
           global_counter2 = 0;
        }
    DAC->DHR12R1 = ((a1*wavetable[global_counter1++])/2) + ((a2*wavetable[global_counter2])/2);
    global_counter2 += 2;

    // Trigger the conversion
    DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
}

void play_sound(char note, char duration){
    int freq, dur;

    // 7th octave gives more room for error
    if (note == 'A'){
        freq = 1760;
    }
    else if (note == 'B'){
        freq = 1976;
    }
    else if (note == 'C'){
        freq = 2093;
    }
    else if (note == 'D'){
        freq = 2349;
    }
    else if (note == 'E'){
        freq = 2637;
    }
    else if (note == 'F'){
        freq = 2794;
    }
    else if (note == 'G'){
        freq = 3136;
    }

    TIM2->PSC = 100 - 1;
    TIM2->ARR = (480000 / freq) - 1;
    TIM2->CNT = 0;

    if (duration == 'w'){
        dur = 1000000;
    }
    else if (duration == 'h'){
        dur = 1000000 / 2;
    }
    else if (duration == 'q'){
        dur = 1000000 / 4;
    }
    else if (duration == 'e'){
        dur = 1000000 / 8;
    }
    else if (duration == 's'){
        dur = 1000000 / 16;
    }

    micro_wait(dur);
//    nano_wait(dur<<3);

    DAC->CR &= ~DAC_CR_EN1;
    micro_wait(10000);       // 10 ms?
//    nano_wait(10000<<3);
    DAC->CR |= DAC_CR_EN1;
    return;

}

void tetris(void)
{
    serial_init();
    setup_gpio();
    setup_dac();
    setup_timer2();

// tetris sheet: https://musescore.com/user/16693/scores/38133
// treble clef to identify notes
// Tetris Korobeiniki https://www.piano-keyboard-guide.com/how-to-play-the-tetris-theme-song-easy-piano-tutorial-korobeiniki/


    play_sound('E', 'q');
    play_sound('B', 'e');
    play_sound('C', 'e');
    play_sound('D', 'q');
    play_sound('C', 'e');
    play_sound('B', 'e');

    play_sound('A', 'q');
    play_sound('A', 'e');
    play_sound('C', 'e');
    play_sound('E', 'q');
    play_sound('D', 'e');
    play_sound('C', 'e');

    play_sound('B', 'q');
    play_sound('B', 'q'); // missing?
    play_sound('C', 'e');
    play_sound('D', 'e');
    play_sound('E', 'q');

    play_sound('C', 'q');
    play_sound('A', 'q');
    play_sound('A', 'q');
    play_sound('r', 'q'); // rest, then second line

    play_sound('D', 'q');
        play_sound('D', 'e'); // this note is 1.5
    play_sound('F', 'e');
    play_sound('A', 'q');
    play_sound('G', 'e');
    play_sound('F', 'e');

    play_sound('C', 'q');
        play_sound('C', 'e');
    play_sound('E', 'e');
    play_sound('D', 'q');
    play_sound('C', 'e');
    play_sound('B', 'e');

    play_sound('B', 'q');
    play_sound('C', 'e');
    play_sound('D', 'e');
    play_sound('E', 'q');
    play_sound('C', 'q');

    play_sound('A', 'q');
    play_sound('A', 'q');
    play_sound('r', 'q'); // rest

    // notes aren't quite right below line 236.
    micro_wait(1000000);
//    nano_wait(1000000000);
    DAC->CR &= ~DAC_CR_EN1;         // TURN IT OFF. PLEASE TURN IT OFF!!!!
//    return;
}


