#ifndef LCD_H_
#define LCD_H_

#include "lib/Nano100Series.h"


typedef struct
{
    uint32_t Sub_Zone_Num;      /*!< Sub zone number */
    uint32_t Zone_Digit_SegNum; /*!< Segment number */
} ZoneInfo_TypeDef;


extern char *Zone[];
extern const ZoneInfo_TypeDef LCD_ZoneInfo[];
extern const uint16_t *Zone_TextDisplay[];


#define CyclesPerUs __HSI / 1000000


#endif