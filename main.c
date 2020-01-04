#include "lib/Nano100Series.h"
#include "lib/LCDLIB.h"
#include "lib/sys.h"
#include "lib/rtc.h"

void main() {
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

    /* LCD Open */
    //LCD_Open(LCD_C_TYPE, 4, LCD_BIAS_THIRD, LCD_FREQ_DIV64, LCD_CPVOl_3V);

    /* IP reset */
    SYS->IPRST_CTL2 |= SYS_IPRST_CTL2_LCD_RST_Msk;
    SYS->IPRST_CTL2 &= ~SYS_IPRST_CTL2_LCD_RST_Msk;

    /* Enable LCD */
    LCD->CTL &= ~LCD_CTL_EN_Msk;

    /* Turn everything off */
    LCD->MEM_0 = u32SetValue;
    LCD->MEM_1 = u32SetValue;
    LCD->MEM_2 = u32SetValue;
    LCD->MEM_3 = u32SetValue;
    LCD->MEM_4 = u32SetValue;
    LCD->MEM_5 = u32SetValue;
    LCD->MEM_6 = u32SetValue;
    LCD->MEM_7 = u32SetValue;
    LCD->MEM_8 = u32SetValue;



    /* Enable LCD */
    LCD->CTL |= LCD_CTL_EN_Msk;

    LCDLIB_Printf(0, "SOC2019");
    while(1);
}
