.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp


.global micro_wait
micro_wait:
			push {lr}
top:
            movs r1, #10    // 1 cycle
loop:       subs r1, #1     // 1 cycle
            bne loop        // 3 cycles
            nop             // 1 cycle
            nop             // 1 cycle
            nop             // 1 cycle
            subs r0, #1     // 1 cycle
            bne  top        // 3 cycles
            pop  {pc}       // 1 cycle

            // Total delay = r0 * (1 + 10*(1+3) + 1 + 1 + 1 + 1 + 3) + 1
