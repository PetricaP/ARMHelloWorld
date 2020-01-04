#include "LCD.h"
#include "lib/lcd.h"
#include "lib/sys.h"
#include "lib/rtc.h"
#include "lib/clk.h"
#include <string.h>


void LCD_SetPixel(uint32_t u32Com, uint32_t u32Seg, uint32_t u32OnFlag);


void LCD_PrintString(uint32_t  u32Zone, char *string)
{
    int      data, length, index;
    uint16_t bitfield;
    uint32_t com, bit;
    int      i;

    length = strlen(string);
    index  = 0;

    /* fill out all characters on display */
    for (index = 0; index < LCD_ZoneInfo[u32Zone].Sub_Zone_Num; index++)
    {
        if (index < length)
        {
            data = (int) *string;
        }
        else           /* padding with space */
        {
            data = 0x20; /* SPACE */
        }
        /* defined letters currently starts at "SPACE" - 0x20; */
        data     = data - 0x20;
        bitfield = *(Zone_TextDisplay[u32Zone] + data);

        for (i = 0; i < LCD_ZoneInfo[u32Zone].Zone_Digit_SegNum; i++)
        {
            bit = *(Zone[u32Zone]
                    + index*LCD_ZoneInfo[u32Zone].Zone_Digit_SegNum*2
                    + i*2 + 1);

            com = *(Zone[u32Zone]
                    + index*LCD_ZoneInfo[u32Zone].Zone_Digit_SegNum*2
                    + i*2 + 0);

            LCD_SetPixel(com, bit, 0);

            if (bitfield & (1 << i))
            {
                /* Turn on segment */
                LCD_SetPixel(com, bit, 1);
            }
        }
        string++;
    }
}


static inline void LCD_Init() {
    /* Unlock protected registers */
    while(SYS->RegLockAddr != SYS_RegLockAddr_RegUnLock_Msk) {
        SYS->RegLockAddr = 0x59;
        SYS->RegLockAddr = 0x16;
        SYS->RegLockAddr = 0x88;
    }

    /* Select IP clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART_S_Msk;
    CLK->CLKSEL1 |= (0x0 << CLK_CLKSEL1_UART_S_Pos);// Clock source from external 12 MHz or 32 KHz crystal clock

    CLK->CLKSEL1 &= ~CLK_CLKSEL1_LCD_S_Msk;
    CLK->CLKSEL1 |= (0x0 << CLK_CLKSEL1_LCD_S_LXT);// Clock source from external 12 MHz or 32 KHz crystal clock

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_LCD_EN;

    /* Select LCD COMs, SEGs, V1 ~ V3, DH1, DH2 */
    SYS->PA_L_MFP |= 0x77770000;    /* seg 36 ~ 39 */
    SYS->PA_H_MFP |= 0x7777;        /* seg 20 ~ 23 */
    SYS->PB_L_MFP = 0x77777777;     /* seg 10 ~ 13, 4 ~ 7 */
    SYS->PB_H_MFP = 0x77777777;     /* LCD V1 ~ V3, seg 30 ~ 31, 24 ~ 26 */
    SYS->PC_L_MFP |= 0x777777;      /* LCD COM3 ~ COM0, DH1/DH2 */
    SYS->PC_H_MFP |= 0x77000000;    /* seg 32 ~ 33 */
    SYS->PD_L_MFP |= 0x77770000;    /* seg 2 ~ 3, 34 ~ 35 */
    SYS->PD_H_MFP = 0x77777777;     /* seg 0 ~ 1, 14 ~ 19 */
    SYS->PE_L_MFP |= 0x70000000;    /* seg 8 */
    SYS->PE_H_MFP |= 0x77700007;    /* seg 9, 27 ~ 29 */

    /* Lock protected registers */
    SYS->RegLockAddr = 0;


    /* IP reset */
    SYS->IPRST_CTL2 |= SYS_IPRST_CTL2_LCD_RST_Msk;
    SYS->IPRST_CTL2 &= ~SYS_IPRST_CTL2_LCD_RST_Msk;

    /* Enable LCD */
    LCD->CTL &= ~LCD_CTL_EN_Msk;

    /* Turn everything off */
    LCD->MEM_0 = 0;
    LCD->MEM_1 = 0;
    LCD->MEM_2 = 0;
    LCD->MEM_3 = 0;
    LCD->MEM_4 = 0;
    LCD->MEM_5 = 0;
    LCD->MEM_6 = 0;
    LCD->MEM_7 = 0;
    LCD->MEM_8 = 0;

    /* Wait a little bit for the values to sink */
    SysTick->LOAD = 300 * CyclesPerUs;

    SysTick->VAL  =  (0x00);
    /* Set the clock source to internal and enable ARM SysTick */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    /* Waiting for down-count to zero */
    while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);

    /* Configure LCD bias and enable charge pump */
    // set internal source for charge pump
    LCD->DISPCTL &= ~LCD_DISPCTL_BV_SEL_Msk;
    // reset charge pump frequency to system clock
    LCD->DISPCTL = LCD->DISPCTL & ~LCD_DISPCTL_CPUMP_FREQ_Msk;
    // set churge pump voltage level to 3V
    LCD->DISPCTL = LCD->DISPCTL & ~LCD_DISPCTL_CPUMP_VOL_SET_Msk | LCD_CPVOl_3V;
    // disable bias reference ladder
    LCD->DISPCTL &= ~LCD_DISPCTL_IBRL_EN_Msk;
    // enable charge pump
    LCD->DISPCTL |= LCD_DISPCTL_CPUMP_EN_Msk;

    // Reset frame rate for LCD
    LCD->CTL &= ~LCD_CTL_FREQ_Msk;
    // Set the desired frame rate
    LCD->CTL |= LCD_FREQ_DIV64;

    // Set LCD mux according to the number of COMS we have
    // In this case 4 COMS -> 1/4 duty
    LCD->CTL = (LCD->CTL & ~LCD_CTL_MUX_Msk) | (3 << LCD_CTL_MUX_Pos);

    // Set bias level to 1 / 3 bias
    LCD->DISPCTL = LCD->DISPCTL & ~LCD_DISPCTL_BIAS_SEL_Msk | LCD_BIAS_THIRD;

    /* Enable LCD */
    LCD->CTL |= LCD_CTL_EN_Msk;
}


void main() {
    LCD_Init();

    LCD_PrintString(0, "SOC2019");
    while(1);
}
