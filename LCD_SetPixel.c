#include "LCD.h"
#include "lib/lcd.h"
#include "lib/sys.h"
#include "lib/rtc.h"
#include "lib/clk.h"


void LCD_SetPixel(uint32_t u32Com, uint32_t u32Seg, uint32_t u32OnFlag)
{
    int32_t memnum = u32Seg / 4;
    int32_t seg_shift = 8*(u32Seg-(4*memnum));

    volatile uint32_t *MEM_BASE = &LCD->MEM_0;
    const uint32_t mask = (1 << u32Com) << seg_shift;
    if(u32OnFlag)
    {
        *(MEM_BASE + memnum) |= mask;
    }
    else
    {
        *(MEM_BASE + memnum) &= ~mask;
    }

    SysTick->LOAD = 300 * CyclesPerUs;

    SysTick->VAL  =  (0x00);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    /* Waiting for down-count to zero */
    while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
}