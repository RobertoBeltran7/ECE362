.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

.global main
main:

//******************************************************************************
// Code Segment 1:
//
// Description:
//    For this section you will write an assembly program to calculate
//    the length of a string. Remember that a string ends with '\0'
//    whose value is 0. The first character of the string is stored at
//    the address 'str' (i.e. label). You will need to read each
//    character one byte at a time (Hint: use ldrb) and compare it with 0.
//    If it is not zero, increment the length of the string. Store the
//    result in r0. Type in your code under codeSegment1. You will most
//    likely use a while loop for this program whose structure would resemble:
//
//    loop1: Check for string termination
//           Branch out of loop if done
//           Perform action
//           Branch back to loop1
//    done1:
//
// Useful Instructions:
//    beq, cmp, adds, ldrb, movs, ldr, b
//
//*****************************************************************************

codeSegment1:
	movs r0, #0
	movs r3, #0
	loop1:	ldr r1,=str
			ldrb r2,[r1,r0]
			cmp r2,r3
			beq done1
			adds r0,#1
			b loop1
			bkpt
	done1:	wfi

.data
str: .string "test string"

//*****************************************************************************
// Code Segment 2:
//
// Description:
//    For this section your will read the word at 'bitPattern' and
//    identify the location of the most significant bit. For example if
//    value at bitPattern is 0x00010002 the most bit is at bit sixteen,
//    so the result of this should be 16 Store the result in r0. Type in
//    your code under codeSegment2.
//
//    You will most likely use a while loop for this program whose structure
//    would resemble:
//
//    loop2: Check for loop termination
//           Branch out of loop if done
//           Perform action
//           Branch back to loop2
//    done2:
//
// Useful Instructions:
//    bcs, lsls, subs, adds, movs, ldr, b
//
//*****************************************************************************

.text
codeSegment2:
    movs r0,#0
    movs r1,#0
    movs r2,#0
    ldr r1,=bitPattern
    ldr r2,[r1]
    loop2:
    		lsls r2,r2, #1
    		bcs done2
    		adds r0,#1
			b loop2
    done2: adds r0,#1
    		wfi

.data
.align 4
bitPattern: .word  0x10A01001

//*****************************************************************************
// Code Segment 3:
//
// Description:
//   For this section you will write a simple program to read the elements
//   of an array from 'src', of size given by 'arrSize', and copy
//   that to 'dest' in reverse order. The last element of 'src'
//   array with be the first in 'dest' and first in 'src' will be
//   the last element in 'dest'.  The 'arrSize' stores the size
//   of the 'src' array in bytes. Type your program under codeSegment3.
//   Note use ldrb to copy elements.
//
//   Hint: For this program you will need to maintain two counters, one for
//   source and the other for destination, in your loop you will increment one
//   and decrement the other while copying the content from src to dest.
//
// Useful Instructions:
//   ldr, movs, subs, cmp, ldrb, strb, adds, b, beq
//
//*****************************************************************************

.text
codeSegment3:
	movs r0,#0
	ldr r1,=arrSize
	ldr r1,[r1]
	subs r1,#1
	ldr r2,=src
	ldr r3,=dest

	loop3:	ldrb r4,[r2,r1]
			strb r4,[r3,r0]
			subs r1,#1
			adds r0,#1
			cmp r0,#4
			beq done3
			b loop3

	done3:	wfi

.data
.align 4
arrSize: .word 4
src:     .word 0xDEADBEEF
         .word 0xABADCAFE
         .word 0xBAADF00D
         .word 0xCAFED00D
         .word 0xDEADC0DE
.align 4
dest:    .space 13

//*****************************************************************************
// Code Segment 4:
//
// Description:
//    Read a byte from 'char' assume that the stored value is
//    alphabetic, and check if the character is uppercase or lowercase
//    Hint: ASCII values of lowercase alphabets are greater than 0x60.
//    If the character is lowercase, set the value at label 'lower'
//    to 0xFF else set it to 0x00. Type your code under codeSegment4.
//
// Useful Instructions:
//    ldr, movs, bge, strb, b
//
//*****************************************************************************

.text
codeSegment4:
    movs r0,#0
    movs r1,#0
    movs r2, #0x60
    movs r6,#0
    loop4:	ldr r0,=char
    		ldrb r1,[r0]
    		cmp r1,r2
    		bgt lowercase
    		b uppercase
    lowercase:	movs r3,#0xff
    			ldr r4,=lower
    			strb r3,[r4]
    			ldr r4,=lower
    			ldrb r4,[r4]
    			b done4
    uppercase:	ldr r5,=#0x00
    			ldr r6,=lower
    			strb r5,[r6]
    			ldr r6,=lower
    			ldrb r6,[r6]
    			b done4
	done4:	wfi

.data
.align 4
char:   .string "b"

.align 4
lower:  .space 1

//*****************************************************************************
// Code Segment 5:
//
// Description:
//     Given an array of whose length is stored at "len", iterate through the
//     array at "arr", to find the maximum value in the array, assume all
//     the values are unsigned 32 bit integers. Store the maximum value at
//     "max". Caution, "len" is the number of bytes and NOT the number of
//     elements in the array.
//
// Useful Instructions:
//     ldr, movs, bge, bgt, b, adds, cmp
//
//*****************************************************************************

.text
codeSegment5:
    movs r0,#0
    ldr r1,=arr
    ldr r4,=max
    ldrb r5,[r4]
    ldr r3,=len
    ldrb r6,[r3]

    loop5:	ldrb r2,[r1,r0]
    		cmp r2,r5
    		bgt maxval
    		cmp r0,r6
    		bge done5
    		adds r0,#4
			b loop5

	maxval:	movs r5,r2
			strb r5,[r4]
			adds r0,#4
			b loop5

	done5:	ldr r7,=max
			ldrb r7,[r7]
			wfi

.data
.align 4
len:     .word 20
arr:     .word 0x01
         .word 0x06
         .word 0x03
         .word 0x04
         .word 0x09
.align 4
max:    .space 4

//*****************************************************************************
//
//    Infinite loop
//
//*****************************************************************************
.text
inf_loop:
    nop
    b inf_loop
// We never reach this point.
