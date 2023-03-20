/**
  ******************************************************************************
  * @file    PWR/PWR_CurrentConsumption/stm32f4xx_lp_modes.c 
  * @author  MCD Application Team
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the STM32F4xx Low Power Modes:
  *           - Sleep Mode
  *           - STOP mode with RTC
  *           - Under-Drive STOP mode with RTC
  *           - STANDBY mode without RTC and BKPSRAM
  *           - STANDBY mode with RTC
  *           - STANDBY mode with RTC and BKPSRAM
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup PWR_CurrentConsumption
  * @{
  */ 

#define PHY_PWR_DOWN       (1<<11)
#define PHY_ADDRESS        0x00 /* default ADDR for PHY: LAN8742 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* RTC handler declaration */
RTC_HandleTypeDef RTCHandle;

/* Private function prototypes -----------------------------------------------*/
//static void SYSCLKConfig_STOP(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures the system to enter Sleep mode for
  *         current consumption measurement purpose.
  *         Sleep Mode
  *         ==========  
  *            - System Running at PLL (100MHz)
  *            - Flash 3 wait state
  *            - Instruction and Data caches ON
  *            - Prefetch ON   
  *            - Code running from Internal FLASH
  *            - All peripherals disabled.
  *            - Wakeup using EXTI Line (user Button)
  * @param  None
  * @retval None
  */
void SleepMode_Measure(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Disable USB Clock */
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

  /* Disable Ethernet Clock */
  __HAL_RCC_ETH_CLK_DISABLE();

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  //__HAL_RCC_GPIOB_CLK_ENABLE();
  //__HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  //HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  //HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  //__HAL_RCC_GPIOB_CLK_DISABLE();
  //__HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
  __HAL_RCC_GPIOJ_CLK_DISABLE();
  __HAL_RCC_GPIOK_CLK_DISABLE();


  /* Suspend Tick increment to prevent wakeup by Systick interrupt. 
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
  
  /* Exit Ethernet Phy from LowPower mode */
  //ETH_PhyExitFromPowerDownMode();
}






    
///**
//  * @brief  Configures system clock after wake-up from STOP: enable HSI, PLL
//  *         and select PLL as system clock source.
//  * @param  None
//  * @retval None
//  */
//static void SYSCLKConfig_STOP(void)
//{
//  RCC_ClkInitTypeDef RCC_ClkInitStruct;
//  RCC_OscInitTypeDef RCC_OscInitStruct;
//  uint32_t pFLatency = 0;
//  
//  /* Get the Oscillators configuration according to the internal RCC registers */
//  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
//  
//  /* After wake-up from STOP reconfigure the system clock: Enable HSE and PLL */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }

//  /* Get the Clocks configuration according to the internal RCC registers */
//  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &pFLatency);
//  
//  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
//     clocks dividers */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, pFLatency) != HAL_OK)
//  {
//    Error_Handler();
//  }
//}

///**
//  * @brief  This function configures the ETH PHY to enter the power down mode
//  *         This function should be called before entering the low power mode.
//  * @param  None
//  * @retval None
//  */
//void ETH_PhyEnterPowerDownMode(void)
//{
//  ETH_HandleTypeDef heth;
//  GPIO_InitTypeDef GPIO_InitStruct;
//  uint32_t phyregval = 0; 
//   
//  /* This part of code is used only when the ETH peripheral is disabled 
//	   when the ETH is used in the application this initialization code 
//	   is called in HAL_ETH_MspInit() function  ***********************/
//	
//	/* Enable GPIO clocks*/
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  
//  /* Configure PA2: ETH_MDIO */
//  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL; 
//  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
//  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//  
//  /* Configure PC1: ETH_MDC */
//  GPIO_InitStruct.Pin = GPIO_PIN_1;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//  
//  /* Enable the ETH peripheral clocks */
//  __HAL_RCC_ETH_CLK_ENABLE();
//  
//  /* Set ETH Handle parameters */
//  heth.Instance = ETH;
//  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;

//  /* Configure MDC clock: the MDC Clock Range configuration
//	   depends on the system clock: 180Mhz/102 = 1.76MHz  */
//  /* MDC: a periodic clock that provides the timing reference for 
//	   the MDIO data transfer which shouldn't exceed the maximum frequency of 2.5 MHz.*/
//  heth.Instance->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div102;

//  /*****************************************************************/
//  
//  /* ETH PHY configuration in Power Down mode **********************/ 
//  
//  /* Read ETH PHY control register */
//  HAL_ETH_ReadPHYRegister(&heth, PHY_BCR, &phyregval);
//  
//  /* Set Power down mode bit */
//  phyregval |= PHY_POWERDOWN;
//  
//  /* Write new value to ETH PHY control register */
//  HAL_ETH_WritePHYRegister(&heth, PHY_BCR, phyregval);
//  
//  /*****************************************************************/
//  
//  /* Disable periph CLKs */
//  __HAL_RCC_GPIOA_CLK_DISABLE();
//  __HAL_RCC_GPIOC_CLK_DISABLE();
//  __HAL_RCC_ETH_CLK_DISABLE();
//}
///**
//  * @brief  This function wakeup the ETH PHY from the power down mode
//  *         When exiting from StandBy mode and the ETH is used in the example
//  *         its better to call this function at the end of HAL_ETH_MspInit() 
//  *         then remove the code that initialize the ETH CLKs ang GPIOs.
//  * @param  None
//  * @retval None
//  */
//void ETH_PhyExitFromPowerDownMode(void)
//{
//   ETH_HandleTypeDef heth;
//   GPIO_InitTypeDef GPIO_InitStruct;
//   uint32_t phyregval = 0;
//   
//  /* ETH CLKs and GPIOs initialization ******************************/
//  /* To be removed when the function is called from HAL_ETH_MspInit() when 
//     exiting from Standby mode */
//	
//	/* Enable GPIO clocks*/
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  
//  /* Configure PA2 */
//  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL; 
//  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
//  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//  
//  /* Configure PC1*/
//  GPIO_InitStruct.Pin = GPIO_PIN_1;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//  
//  /* Enable ETH CLK */
//  __HAL_RCC_ETH_CLK_ENABLE();
//  /*****************************************************************/
//  
//  /* ETH PHY configuration to exit Power Down mode *****************/
//  /* Set ETH Handle parameters */
//  heth.Instance = ETH;
//  heth.Init.PhyAddress = LAN8742A_PHY_ADDRESS;
//  
//  /* Configure MDC clock: the MDC Clock Range configuration
//	   depends on the system clock: 180Mhz/102 = 1.76MHz  */
//  /* MDC: a periodic clock that provides the timing reference for 
//	   the MDIO data transfer which shouldn't exceed the maximum frequency of 2.5 MHz.*/
//  heth.Instance->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div102; 
//	
//  /* Read ETH PHY control register */
//  HAL_ETH_ReadPHYRegister(&heth, PHY_BCR, &phyregval);
//  
//  /* check if the PHY  is already in power down mode */
//  if ((phyregval & PHY_POWERDOWN) != RESET)
//  {
//    /* Disable Power down mode */
//    phyregval &= ~ PHY_POWERDOWN;
//    
//    /* Write value to ETH PHY control register */
//    HAL_ETH_WritePHYRegister(&heth, PHY_BCR, phyregval);
//  }
//  /*****************************************************************/
//}

///**
//  * @}
//  */ 

///**
//  * @}
//  */ 
