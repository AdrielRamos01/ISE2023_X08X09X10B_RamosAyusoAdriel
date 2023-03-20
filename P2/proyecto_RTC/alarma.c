#include "alarma.h"
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os2.h"                          // CMSIS RTOS header file
/*----------------------------------------------------------------------------
 *      Timer_Alarm: Wake Up LED1 for 5 seconds
 *---------------------------------------------------------------------------*/
 
 
/*----- Periodic Timer Example -----*/
osTimerId_t tim_id2;                            // timer id
osTimerId_t tim_id1;                            // timer id

static uint32_t exec2;                          // argument for the timer call back function
static uint32_t exec1;                          // argument for the timer call back function

// Periodic Timer Function
int tics=0;
int tics2=0;

void Timer1_Callback (void const *arg) {
  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
	tics2++;
	if (tics2 == 8){
		osTimerStop(tim_id1);
		tics2 = 0;
	}
	
}

void Timer2_Callback (void const *arg) {
  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
	tics++;
	if (tics == 20){
		osTimerStop(tim_id2);
		tics = 0;
	}
	
}
 
int Init_Timer_Update (void) {

  // Create periodic timer
  exec1 = 1U;
  tim_id1 = osTimerNew((osTimerFunc_t)&Timer1_Callback, osTimerPeriodic, &exec1, NULL);
  osTimerStart(tim_id1, 250U);   

  return NULL;
}


int Init_Timer_Alarm (void) {

  // Create periodic timer
  exec2 = 2U;
  tim_id2 = osTimerNew((osTimerFunc_t)&Timer2_Callback, osTimerPeriodic, &exec2, NULL);
  osTimerStart(tim_id2, 250U);

  return NULL;
}
