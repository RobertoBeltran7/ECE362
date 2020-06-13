.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

.global micro_wait
micro_wait:
            // Total delay = r0 * (1 + 10*(1+3) + 1 + 1 + 1 + 1 + 3) + 1
            //             = r0 * 48 cycles
            // At 48MHz, this is one usec per loop pass.
            // Maximum delay is 2^31 usec = 2147.5 sec.
            movs r1, #10    // 1 cycle
loop:       subs r1, #1     // 1 cycle
            bne loop        // 3 cycles
            nop             // 1 cycle
            nop             // 1 cycle
            nop             // 1 cycle
            subs r0, #1     // 1 cycle
            bne  micro_wait // 3 cycles
            bx  lr          // 1 cycle

.global nano_wait
nano_wait:
		// Each pass through loop takes 4 cycles
		// at 48MHz, each cycle is 20.83nsec.
		// 4 cycles is about 83.33nsec.
		// Subtract 83 from the counter each step.
		// Maximum delay is 2^31 nsec = 2.147 sec.
    	subs r0, #83    // 1 cycle
    	bge  nano_wait  // 3 cycles
    	bx   lr			// 1 cycle
