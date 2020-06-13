#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

#define FAIL -1
#define SUCCESS 0
#define WR 0
#define RD 1

void serial_init(void);
void test_prob3(void);
void prob5(void);
void check_config(void);
void test_wiring(void);
void test_prob4(void);

//===========================================================================
// Check wait for the bus to be idle.
void I2C1_waitidle(void) {
    while ((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);  // while busy, wait.
}

//===========================================================================
// Subroutines for step 2.
//===========================================================================
// Initialize I2C1
/*
1.  Enable clock to GPIOB
2.  Configure PB6 and PB7 to alternate functions I2C1_SCL and I2C1_SDA
3.  Enable clock to I2C1
4.  Set I2C1 to 7 bit mode
5.  Enable NACK generation for I2C1
6.  Configure the I2C1 timing register so that PSC is 6, SCLDEL is 3 and SDADEL is 1 and SCLH is 3 and SCLL is 1
7.  Disable own address1 and own address 2 and set the 7 bit own address to 1
8.  Enable I2C1
 */
void init_I2C1() {
   
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~GPIO_MODER_MODER6;
	GPIOB->MODER |= GPIO_MODER_MODER6_1;
	GPIOB->MODER &= ~GPIO_MODER_MODER7;
	GPIOB->MODER |= GPIO_MODER_MODER7_1;
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL6;
	GPIOB->AFR[0] |= 0x1000000;
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFRL7;
	GPIOB->AFR[0] |= 0x10000000;
	
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	I2C1->CR2 &= ~I2C_CR2_ADD10;
	I2C1->CR2 |= I2C_CR2_NACK;
	I2C1->TIMINGR &= ~I2C_TIMINGR_PRESC;
	I2C1->TIMINGR |= 4 << 28;
	I2C1->TIMINGR &= ~I2C_TIMINGR_SCLDEL;
	I2C1->TIMINGR |= 3 << 20;
	I2C1->TIMINGR &= ~I2C_TIMINGR_SDADEL;
	I2C1->TIMINGR |= 1 << 16;
	I2C1->TIMINGR &= ~I2C_TIMINGR_SCLH;
	I2C1->TIMINGR |= 3 << 8;
	I2C1->TIMINGR &= ~I2C_TIMINGR_SCLL;
	I2C1->TIMINGR |= 9 << 0;
	I2C1->OAR1 &= ~I2C_OAR1_OA1EN;
	I2C1->OAR2 &= ~I2C_OAR2_OA2EN;
	I2C1->OAR1 = I2C_OAR1_OA1EN | 0x2;
	I2C1->CR1 |= I2C_CR1_PE;;

}


//===========================================================================
// Subroutines for step 3.
//===========================================================================
void I2C1_start(uint8_t addr, uint32_t dir) {
    
//	I2C1->CR2 &= ~I2C_CR2_SADD;
//	I2C1->CR2 |= addr << 1;
//	if(dir == RD)
//		I2C1->CR2 |= I2C_CR2_RD_WRN;
//	else
//		I2C1->CR2 &= ~I2C_CR2_RD_WRN;
//	I2C1->CR2 |= I2C_CR2_START;

	uint32_t tmpreg = I2C1->CR2;
	tmpreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES |
	I2C_CR2_RELOAD | I2C_CR2_AUTOEND |
	I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
	if (dir == 1)
		tmpreg |= I2C_CR2_RD_WRN; // Read from slave
	else
		tmpreg &= I2C_CR2_RD_WRN; // Write to slave
	tmpreg |= ((addr<<1) & I2C_CR2_SADD);
	tmpreg |= I2C_CR2_START;
	I2C1->CR2 = tmpreg;

   
}

void I2C1_stop() {
    
	if (I2C1->ISR & I2C_ISR_STOPF)
		return;
		
	// Master: Generate STOP bit after current byte has been transferred.
	I2C1->CR2 |= I2C_CR2_STOP;
	
	// Wait until STOPF flag is reset
	while( (I2C1->ISR & I2C_ISR_STOPF) == 0);
	I2C1->ICR |= I2C_ICR_STOPCF; // Write to clear STOPF flag
  
}

int I2C1_senddata(uint8_t* data, uint32_t size) {
    
	I2C1->CR2 &= ~I2C_CR2_NBYTES;
	I2C1->CR2 |= (size & 0xFF) << 16;
	for(int i = 0; i < size; i++){
		int timeout = 0;
		while((I2C1->ISR & I2C_ISR_TXIS) == 0){
			timeout++;
			if(timeout > 1000000)
				return FAIL;
		}
		I2C1->TXDR = data[i];
	}
	while(((I2C1->ISR & I2C_ISR_TC) == 0) && ((I2C1->ISR & I2C_ISR_NACKF) == 0));
	if((I2C1->ISR & I2C_ISR_NACKF))
		return FAIL;
	else
		return SUCCESS;

}

int I2C1_readdata(int8_t* data, uint32_t size) {
    
	I2C1->CR2 &= ~I2C_CR2_NBYTES;
	I2C1->CR2 |= (size & 0xFF)<< 16;
	for(int i = 0; i < size; i++){
			int timeout = 0;
			while((I2C1->ISR & I2C_ISR_RXNE) == 0){
				timeout++;
				if(timeout > 1000000)
					return FAIL;
			}
			data[i] = I2C1->RXDR;
		}
		while(((I2C1->ISR & I2C_ISR_TC) == 0) && ((I2C1->ISR & I2C_ISR_NACKF) == 0));
		if((I2C1->ISR & I2C_ISR_NACKF) >> 4)
			return FAIL;
		else
			return SUCCESS;
}

//===========================================================================
// Subroutines for step 4.
//===========================================================================
int read_temperature() {

	uint8_t data[1] = {0};

	uint8_t temp = -20;
	I2C1_waitidle();
	I2C1_start(0x4D, WR);
	I2C1_senddata(data,1);
	I2C1_start(0x4D, RD);
	I2C1_readdata(&temp, 1);
	I2C1_stop();
	return temp;
   
}


//===========================================================================
// Subroutines for step 5.
//===========================================================================
void write_EEPROM(uint16_t wr_addr, uint8_t data) {
    
	uint8_t write_buf[3];
	write_buf[0] = (wr_addr & 0xFF00) >> 8;
	write_buf[1] = (wr_addr & 0xFF);
	write_buf[2] = data;
	I2C1_waitidle();
	I2C1_start(0x50, WR);
	I2C1_senddata(write_buf, 3);
	I2C1_stop();
	micro_wait(5000);

}

void prob2() {
    init_I2C1();
    check_config();
}

void prob3() {
    test_prob3();
}

void prob4() {
    test_prob4();
}


int main(void)
{
    serial_init();
    //Open the serial terminal to see messages for each part
    //test_wiring();
    //prob2();
    //prob3();
    prob4();
    //prob5();

    while(1);
}
