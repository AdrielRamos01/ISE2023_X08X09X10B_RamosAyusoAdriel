
#include "rtc.h"
#include "alarma.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "rl_net_ds.h"
#include <time.h>

/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;
RTC_AlarmTypeDef alarm={0};


char lcd_rtc[2][21];

void RTC_Init(void){
  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
   // Error_Handler();
  }
  


  /*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_CalendarConfig();
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
      /* Turn on LED2: Power on reset occurred */
    //  BSP_LED_On(LED2);
    }
    /* Check if Pin Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
      /* Turn on LED1: External reset occurred */
    //  BSP_LED_On(LED1);
    }
    /* Clear source Reset Flag */
   // __HAL_RCC_CLEAR_RESET_FLAGS();
  }


	  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

void RTC_CalendarConfig(void)
{
    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 0th 2023 */
  sdatestructure.Year = 00;
  sdatestructure.Month = RTC_MONTH_JANUARY;
  sdatestructure.Date = 01;
  sdatestructure.WeekDay = RTC_WEEKDAY_SATURDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
   // Error_Handler();
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0;
  stimestructure.Minutes = 0;
  stimestructure.Seconds = 0;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
   // Error_Handler();
  }

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief RTC MSP Initialization 
  *        This function configures the hardware resources used in this example
  * @param hrtc RTC handle pointer
  * 
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select 
  *        the RTC clock source; in this case the Backup domain will be reset in  
  *        order to modify the RTC Clock source, as consequence RTC registers (including 
  *        the backup registers) and RCC_BDCR register are set to their reset values.
  *             
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTc clock source */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  
  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    //Error_Handler();
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    //Error_Handler();
  }
  
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
}




void RTC_CalendarShow(void)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf(lcd_rtc[0], "%0.2d:%0.2d:%0.2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf(lcd_rtc[1], "%0.2d-%0.2d-%0.2d",sdatestructureget.Date, sdatestructureget.Month, 2000 + sdatestructureget.Year);

}
extern osThreadId_t TID_RTC;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{

	osThreadFlagsSet(TID_RTC,0x111);

	Set_Alarm();
}

void RTC_Alarm_IRQHandler(void)
{
  HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

void Set_Alarm(void){

	alarm.AlarmTime.Seconds = 0x10;
	alarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
	alarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	alarm.Alarm = RTC_ALARM_A;
	alarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_SET;
	alarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
	alarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	alarm.AlarmDateWeekDay = 1;
	
	HAL_RTC_SetAlarm_IT(&RtcHandle, &alarm, RTC_FORMAT_BCD);

}


/************************************************************************/
/**************************    SERVIDOR NTP    **************************/
/************************************************************************/


uint32_t seconds_update;
struct tm time_update;
 
void get_time (void) {
  if (netSNTPc_GetTime (NULL, time_callback) == netOK) {
	//	printf ("SNTP request sent.\n");

		
  }
  else {
  //		printf ("SNTP not ready or bad parameters.\n");

  }
}
 
static void time_callback (uint32_t seconds, uint32_t seconds_fraction) {
  if (seconds == 0) {
  //printf ("Server not responding or bad response.\n");

  }
  else {
		
	RTC_DateTypeDef sdatestructureset;
  RTC_TimeTypeDef stimestructureset;
		 //  printf ("%d seconds elapsed since 1.1.1970\n", seconds);
		seconds_update = seconds;
		time_update = *localtime(&seconds_update);
		
		stimestructureset.Hours = time_update.tm_hour+1;
		stimestructureset.Minutes = time_update.tm_min;
		stimestructureset.Seconds= time_update.tm_sec;
		stimestructureset.TimeFormat = RTC_HOURFORMAT12_AM;
		stimestructureset.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
		stimestructureset.StoreOperation = RTC_STOREOPERATION_RESET;
		
		HAL_RTC_SetTime(&RtcHandle,&stimestructureset,RTC_FORMAT_BIN);
		
		sdatestructureset.Year = time_update.tm_year-100;
		sdatestructureset.Month = time_update.tm_mon + 1;
		sdatestructureset.Date = time_update.tm_mday;
    sdatestructureset.WeekDay=time_update.tm_wday;
    
    
		HAL_RTC_SetDate(&RtcHandle,&sdatestructureset,RTC_FORMAT_BIN);

  }
}

void setDateButton (void){
  
    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;
  
    sdatestructure.Year = 00;
    sdatestructure.Month = RTC_MONTH_JANUARY;
    sdatestructure.Date = 01;
    sdatestructure.WeekDay = RTC_WEEKDAY_SATURDAY;
  
    stimestructure.Hours = 0;
    stimestructure.Minutes = 0;
    stimestructure.Seconds = 0;
    stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN);
    HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);

  
  
}

