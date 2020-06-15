# **ECE 362: Microprocessor Systems and Interfacing**

**Course Description:**
An introduction to microcontroller instruction sets, assembly language programming, microcontroller interfacing, microcontroller peripherals, and embedded system design. 

### **List of Lab Assignments**

- **Lab1: Introduction to Assembly**<br/>
This experiment introduces ARM assembly. The microcontroller used is the 32-bit STM32F051R8T6 MCU (on the STM32F0DISCOVERY Board) that utilizes the Cortex-M0 ARM processor. This experiment implements some simple assembly instructions such MOVS, ADDS, SUBS, MULS, LDR, etc.

- **Lab2: More Practice With Assembly**<br/>
This experiment continues with programming in assembly using the STM32F0 MCU. The experiment implements basic operations such as calculating length of a string, reading words and arrays, and basic manipulation of data in arrays.  

- **Lab3: General Purpose I/O**<br/>
This experiment introduces basic input/output(GPIO) for blinking LEDs on the STM32F0 mcu and the PUSH/POP instructions used for stack memory.

- **Lab4: Introduction to Interrupts/Interrupt Handlers**<br/>
This experiment introduces implementation of interrupts and interrupt handlers in assembly using the STM32F0 mcu. It also continues implementation of POP/PUSH instructions for memory management. 

- **Lab5: Introduction to Embedded C Programming**<br/>
This experiment introduces embedded C programming with the STM32 mcu by implementing functions that set input/outputs(GPIO) to respond to button presses and turn on connected LEDs. This experiment also introduces timers in embedded C.

- **Lab6: Introduction to Analog-to-Digital/Digital-to-Analog Converters(ADC/DAC)**<br/>
This experiment introduces the ADC and DAC present on the STM32 mcu. The experiment uses two waveforms that are read by the ADC and then their amplitdues and frequencies are displayed on a CFAL1602 LCD display. 

- **Lab7: PWM and DAC**<br/>
This experiment introduces PWM and DAC for the STM32 mcu. The experiment uses a kepad that can change the frequency and duty cycle of an RGB led by first inputting them and displaying them on an LCD and then using the DAC to control the RGB LED.

- **Lab8: Introduction to DMA and SPI**<br/>
This experiment introduces DMA and SPI on the STM32 mcu to interact with a CFAL1602 LCD display. The experiment uses both DMA and SPI to display different messages on the LCD display.

- **Lab9: Introduction to I2C**<br/>
This experiment introduces I2C with the STM32 mcu. The experiment uses I2C to read temperature data from a TC74A0-3.3VAT I2C thermal sensor and then uses a serial terminal program to display the information read from the sensor.

- **Lab10: Introduction to USART**<br/>
This experiment introduces USART implementation using a serial terminal program alongside the STM32 mcu. This experiment goes through the implementation of USART using a serial terminal program to display text input by a keyboard. It also uses USART to respond to commands input by the user and displays the appropiate information based on the input command.

- **Mini Project: Pong game using STM32**<br/>
The Mini Project is the final group project for the ECE 362 course where a simple pong game was implemented using the STM32 mcu. The game initializes with a screen that displays a message to both players and then plays a short theme song. A GDM12864H LCD display was used for the pong game along with two WMYCONGCONG analog joysticks to control the paddles. The project makes use of several concepts used in the course such as GPIO, ADCs, DACs, Timers, and Interrupts. **Group Members: Adrian Mo(ADC peripherals), Cory Lee Chilcote(DAC peripherals), Jason Chen(Paddle/ball movement), Roberto Beltran(LCD initialization/configuration)**