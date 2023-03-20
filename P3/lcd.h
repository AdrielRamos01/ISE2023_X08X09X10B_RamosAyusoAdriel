/**
  ******************************************************************************
  * @author  Adriel Ramos y Daniel Santamaría
  * @brief   Header for lcd.c module
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

/* Defines -------------------------------------*/
#define SIZE_MSGQUEUE_LCD 16                     // number of Message Queue Objects

#define WRITE 0
#define CLEAR 1
#define SET_CURSOR 2


/* Includes ------------------------------------------------------------------*/
#include "cmsis_os2.h"  
#include "stm32f4xx_hal.h"
#include "Driver_SPI.h"


 


typedef struct {                                // object data type
  uint8_t line;
	char text1[32];
	char text2[32];
  uint8_t tipo;
	uint8_t posicion;
} MSGQUEUE_LCD_t;



osMessageQueueId_t getModuloLCDQueueID(void);

void LCD_clean(void);
int LCD_write_texto(uint8_t line, char *cadena);
void LCD_update(void);
void	Init_Pins(void);
	void LCD_reset(void);
	int LCD_clear(uint8_t linea);
	void LCD_init(void);

void initModuloLCD (void);

//Thread para testear modulo
int init_Th_LCD_Test(void);



#endif /* __LCD_H */
