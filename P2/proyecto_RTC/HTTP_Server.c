/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"  

// Keil::Device:STM32Cube HAL:Common
#include "leds.h"                         // ::Board Support:LED
#include "lcd.h"
#include "adc.h"
#include "rtc.h"
#include "alarma.h"
//#include "Board_Buttons.h"              // ::Board Support:Buttons
//#include "Board_ADC.h"                  // ::Board Support:A/D Converter
//#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
//#include "GLCD_Config.h"                // Keil.MCBSTM32F400::Board Support:Graphic LCD

//Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//extern GLCD_FONT GLCD_Font_6x8;
//extern GLCD_FONT GLCD_Font_16x24;

extern uint16_t AD_in          (uint32_t ch);
//extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];
extern char lcd_rtc[2] [20+1];




//extern osThreadId_t TID_SNTP;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };


///* Thread IDs */
  osThreadId_t TID_Display;
  osThreadId_t TID_Led;
	osThreadId_t TID_RTC;
  osThreadId_t TID_SNTP;

///* Thread declarations */
  static void BlinkLed (void *arg);
  static void Display  (void *arg);
	static void RTC_time (void *arg);  //RTC es palabra reservada para algo???
	static void SNTP     (void *arg);

__NO_RETURN void app_main (void *arg);



///* Read analog inputs */
uint16_t AD_in (uint32_t ch) {
  int32_t val = 0;

  if (ch == 0) {
    ADC_StartConversion();
    while (ADC_ConversionDone () < 0);
    val = ADC_GetValue();
  }
  return ((uint16_t)val);
}



///*----------------------------------------------------------------------------
//  Thread 'Display': LCD display handler
// *---------------------------------------------------------------------------*/


static __NO_RETURN void Display (void *arg) {

 //initModuloLCD ();
	Init_Pins();
	LCD_reset();
	LCD_init();
	LCD_clean();
	LCD_update();
	while (1){
	
	  //LCD_clean();
		osThreadFlagsWait (0x01U, osFlagsWaitAny, osWaitForever);
		//LCD_write_texto(2, lcd_text[1]);
		//LCD_write_texto(1, lcd_text[0]);
		
		LCD_write_texto(2, lcd_rtc[1]);
		LCD_write_texto(1, lcd_rtc[0]);
		
		LCD_update();
		
		osDelay(250);
	
	}
	
	 
}

///*----------------------------------------------------------------------------
//  Thread 'BlinkLed': Blink the LEDs on an eval board
// *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  uint32_t cnt = 0U;

  (void)arg;

  LEDrun = true;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      LED_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

///*----------------------------------------------------------------------------
//  Thread 'RTC_Time': Shows the time from the RTC
// *---------------------------------------------------------------------------*/
static __NO_RETURN void RTC_time (void *arg) {
	
  RTC_Init();
	RTC_CalendarConfig();
	Set_Alarm();
  
  while(1) {
    
		RTC_CalendarShow();

		osThreadFlagsSet(TID_Display,0x01U);  // flag para representacion lcd
    osDelay(250);
		
		
		if (0x111 == osThreadFlagsWait(0x111,osFlagsWaitAny, 10U)){
		Init_Timer_Alarm();
		}
		
  }
}

///*----------------------------------------------------------------------------
//  Thread 'SNTP': Server NTP
// *---------------------------------------------------------------------------*/
static __NO_RETURN void SNTP (void *arg) {

	osDelay(5000);
	get_time();
  Init_Timer_Update();

  while(1){
    
		osDelay(180000); //actualizacion cada 3 minutillos
		get_time();
    Init_Timer_Update();

  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

    LED_Initialize();
    ADC_Initialize();

  netInitialize ();

  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);
	TID_RTC     = osThreadNew (RTC_time, NULL, NULL);
  TID_SNTP    = osThreadNew (SNTP,     NULL, NULL);

  osThreadExit();
}


