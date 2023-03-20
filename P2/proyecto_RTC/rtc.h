
#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"
#include <stdio.h>


/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

void RTC_CalendarConfig(void);
void RTC_CalendarShow(void);
void RTC_Init(void);
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void Set_Alarm(void);
void setDateButton (void);

//SNTP FUNCIONES
void get_time (void);
static void time_callback (uint32_t seconds, uint32_t seconds_fraction);



#endif
