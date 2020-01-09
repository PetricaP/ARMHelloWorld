        PUBLIC LCD_SetPixel

;; Nuvoton130 has a Cortex-m0, ARM v6-m, Supports only THUMB instructions
        SECTION `.text`:CODE:NOROOT(2)
        THUMB

;; The symbol name has to match the C function name
LCD_SetPixel:
        ;; Save modified registers
        PUSH    {R4 - R7, LR}
        ;; Second argument (u32Seg) is passed in R1 register
        ;; To calculate it divided by 4, shift by 2 to the left, store in R3
        LSRS     R3, R1, #+2
        ;; Multiply memnum (stored in R3) by 4 store in R4
        LSLS     R4, R3, #+2
        ;; u32Seg - 4 * memnum (stored in R4)
        SUBS     R4, R1, R4
        ;; Multiply the rezult by 8 (shift left)
        LSLS     R4, R4, #+3
        ;; Load in R5 the address of the memory mapped LCD segment data
        LDR      R5, LCD_MEM_BASE ;; (0x400b0008)

        ;; Calculate mask
        MOVS     R6, #+4
        MULS     R6, R3, R6
        LDR      R7, [R5, R6]
        MOVS     R6, #+1
        LSLS     R6, R6, R0
        LSLS     R6, R6, R4

        ;; Check if we want to turn it on or off
        CMP      R2, #+0
        BEQ      ResetPixel

SetPixel:
        ;; Set the required pixel
        ORRS     R6, R6, R7
        MOVS     R7, #+4
        MULS     R7, R3, R7
        STR      R6, [R5, R7]
        B        Ready

ResetPixel:
        ;; Reset the required pixel
        BICS     R7, R7, R6
        MOVS     R6, #+4
        MULS     R6, R3, R6
        STR      R7, [R5, R6]

Ready:
        ;; SysTick->LOAD = 300 * CyclesPerUs (225);
        MOVS     R6, #+225
        LSLS     R6, R6, #+4
        LDR      R7, SYS_TICK_LOAD  ;; 0xe000e014
        STR      R6, [R7, #+0]

        ;; SysTick->VAL = 0;
        MOVS     R6, #+0
        LDR      R7, SYS_TICK_VALUE  ;; 0xe000e018
        STR      R6, [R7, #+0]

        ;; SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
        LDR      R6, SYS_TICK_CTRL  ;; 0xe000e010
        MOVS     R7, #+5
        STR      R7, [R6, #+0]

        ;; while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
WaitTimer:
        LDR      R7, [R6, #+0]
        LSLS     R7, R7, #+15
        BPL      WaitTimer

        POP      {R4 - R7, PC}       ;; return
        ;; In THUMB mode, data sections have to be 4 byte aligned, since we have
        ;; 37 instructions, we need one more to make it aligned
        NOP

        DATA
LCD_MEM_BASE:
        DC32     0x400b0008
SYS_TICK_LOAD:
        DC32     0xe000e014
SYS_TICK_VALUE:
        DC32     0xe000e018
SYS_TICK_CTRL:
        DC32     0xe000e010

        END
