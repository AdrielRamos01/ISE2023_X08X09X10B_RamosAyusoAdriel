/**
  ******************************************************************************
  * @author  Adriel Ramos y Daniel Santamaría
  * @brief   lcd.c module
  ******************************************************************************
  */

/* Includes -------------------------------------------------------------------*/
#include "lcd.h"
#include <stdio.h>
#include "string.h"
#include "Arial12x12.h"

/* Private variables -------------------------------------------------------------------*/
unsigned char buffer[512];
int positionL1, positionL2;

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;//variable del SPI

/* Functions prototipes -------------------------------------------------------------------*/
void LCD_reset(void);
void LCD_init(void);
int LCD_clear(uint8_t line);
void LCD_clearL1(void);
void LCD_clearL2(void);
void LCD_clean(void);
void LCD_update(void);

void delay(uint32_t n_microsegundos);

void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);

int LCD_setCursor(uint8_t line, uint8_t position);
void symbolToLocalBuffer(uint8_t line,uint8_t symbol);
int symbolToLocalBuffer_L1(uint8_t symbol);
int symbolToLocalBuffer_L2(uint8_t symbol);

int LCD_write_texto(uint8_t line, char *texto);


int init_Thread_LCD (void);
void thread_LCD(void *argument);

int init_MsgQueue_LCD (void);

void th_LCD_Test(void *argument); 
 


/* ID's------------------------------------------------------- */
osThreadId_t tid_Th_LCD;                        // id del Thread LCD
osThreadId_t tid_Th_LCD_Test;                   // id del Thread LCD TEST

osMessageQueueId_t mid_MsgQueue_LCD;            // id de la cola LCD


/* Private functions -------------------------------------------------------------------*/
/**
  * @brief  Se inicializan los pines usados en el LCD
  * @param  None
  * @retval None
  */ 
void Init_Pins(){
	GPIO_InitTypeDef GPIO_InitStruct;
	/*LCD_RESET Pin A6*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin=GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET); // Lo pongo a 1
	
	/*LCD_CS_N Pin D14*/
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin=GPIO_PIN_14;
	HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET); // Lo pongo a 1
	
	/*LCD_A0 Pin F13*/
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin=GPIO_PIN_13;
	HAL_GPIO_Init(GPIOF,&GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET); // Lo pongo a 1
}


/**
  * @brief  Reset del LCD usando el SPIdriver
  * @param  None
  * @retval None
  */ 
	
void LCD_reset(){
	SPIdrv->Initialize(NULL);
	SPIdrv->PowerControl(ARM_POWER_FULL);
	SPIdrv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_CPOL1_CPHA1|ARM_SPI_DATA_BITS(8),2000000);
	
	// Inicializamos los pines
	Init_Pins();
	
	//Generamos la se?al de reset
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
	delay(2);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
	
	delay(1000);
}

/**
  * @brief  Funcion para crear delays ya que el HAL_Delay est� "prohibido"
  * @param  n_microsegundos es el tiempo de delay definido en us
  * @retval None
  */ 
void delay(uint32_t n_microsegundos){
	//Configurar y arrancar el timer para generar un evento pasasdos n_microsegundos
	TIM_HandleTypeDef htim7; //Timer 7
	__HAL_RCC_TIM7_CLK_ENABLE();
	htim7.Instance=TIM7;
	htim7.Init.Prescaler=48000-1; //48MHz/48000 = 1000Hz
	htim7.Init.Period=(1000-1)*n_microsegundos;
	//HAL_NVIC_EnableIRQ(TIM7_IRQn); // Habilita interrupci?n
	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start(&htim7);
	
	//Esperar a que se active el flag del resgistro de Match correspondiente
	while(!__HAL_TIM_GET_FLAG(&htim7,TIM_FLAG_UPDATE));
	
	//Borrar el flag
	__HAL_TIM_CLEAR_FLAG(&htim7,TIM_FLAG_UPDATE);
	
	//Parar el timer y ponerlo a cero para la siguiente llamada a la funci?n	
	HAL_TIM_Base_DeInit(&htim7);
	HAL_TIM_Base_Stop(&htim7);
	__HAL_TIM_SET_COUNTER(&htim7,0);
}

/**
  * @brief  Funci�n para la escritura de datos en el LCD
  * @param  data es el dato que queremos escribir en el LCD
  * @retval None
  */ 
void LCD_wr_data(unsigned char data){
	ARM_SPI_STATUS status;
	//Seleccionar CS = 0
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
	//Seleccionar A0 = 1
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET); 
	//Escribir un dato (data)
	SPIdrv->Send(&data,sizeof(data));
	//Esperar a que se libere el bus SPI
	do{
		status = SPIdrv->GetStatus();
	}while(status.busy);
	//Seleccionar CS = 1
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
}

/**
  * @brief  Funci�n para la escritura de comandos en el LCD
  * @param  cmd es el comando que queremos escribir en el LCD
  * @retval None
  */ 
void LCD_wr_cmd(unsigned char cmd){
	ARM_SPI_STATUS status;
	//Seleccionar CS = 0
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
	//Seleccionar A0 = 0
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_RESET); 
	//Escribir un dato (data)
	SPIdrv->Send(&cmd,sizeof(cmd));
	//Esperar a que se libere el bus SPI
	do{
		status = SPIdrv->GetStatus();
	}while(status.busy);
	//Seleccionar CS = 1
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
}

/**
  * @brief  Se inicializan los valores usados en el LCD para la visualizaci�n correcta
  * @param  None
  * @retval None
  */ 
void LCD_init(){
	//Display off
	LCD_wr_cmd(0xAE);
	//Fija el valor de la relaci?n de la tensi?n de polarizaci?n del LCD a 1/9
	LCD_wr_cmd(0xA2);
	//El direccionamiento de la RAM de datos del display es la normal
	LCD_wr_cmd(0xA0);
	//El scan en las salidas COM es el normal
	LCD_wr_cmd(0xC8);
	//Fija la relaci?n de resistencias interna a 2
	LCD_wr_cmd(0x22);
	//Power on
	LCD_wr_cmd(0x2F);
	//Display empieza en la l?nea 0
	LCD_wr_cmd(0x40);
	//Display ON
	LCD_wr_cmd(0xAF);
	//Contraste
	LCD_wr_cmd(0x81);
	//Valor Contraste
	LCD_wr_cmd(0x17);
	//Display all points normal
	LCD_wr_cmd(0xA4);
	//LCD Display normal
	LCD_wr_cmd(0xA6);
}

/**
  * @brief  Actualizamos todas las p�ginas del LCD (LCD completo)
  * @param  None
  * @retval None
  */ 

void LCD_update(void){
 int i;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci?n a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci?n a 0
 LCD_wr_cmd(0xB0); // P?gina 0
 
 for(i=0;i<128;i++){
	LCD_wr_data(buffer[i]);
 }
 
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci?n a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci?n a 0
 LCD_wr_cmd(0xB1); // P?gina 1
 
 for(i=128;i<256;i++){
	LCD_wr_data(buffer[i]);
 }
 
 LCD_wr_cmd(0x00); 
 LCD_wr_cmd(0x10); 
 LCD_wr_cmd(0xB2); //P?gina 2
 
 for(i=256;i<384;i++){
	LCD_wr_data(buffer[i]);
 }
 
 LCD_wr_cmd(0x00); 
 LCD_wr_cmd(0x10); 
 LCD_wr_cmd(0xB3); // Pagina 3
 
 
 for(i=384;i<512;i++){
	LCD_wr_data(buffer[i]);
 }
}

/**
  * @brief  Se borran todos los datos escritos en el LCD, resetea el LCD y situa el cursor a la izquierda
  * @param  None
  * @retval None
  */
void LCD_clearL1(){
	uint16_t i;
	
	for(i = 0; i<256; i++){
		buffer[i] = 0;
	}
	LCD_update();
	LCD_setCursor(1,0);
}

/**
  * @brief  Se borran todos los datos escritos en el LCD, resetea el LCD y situa el cursor a la izquierda
  * @param  None
  * @retval None
  */
void LCD_clearL2(){
	uint16_t i;
	
	for(i = 256; i<512; i++){
		buffer[i] = 0;
	}
	LCD_update();
	LCD_setCursor(2,0);
}


/**
  * @brief  Se borran todos los datos escritos en el LCD, resetea el LCD y situa el cursor a la izquierda
  * @param  None
  * @retval None
  */
int LCD_clear(uint8_t line){

	if (line != 1 && line != 2){
	  return 1;
	}
	
		if(line==1)
		LCD_clearL1();
	else if (line==2)
		LCD_clearL2();
	
	return 0;
	
}



/**
  * @brief  Se borran todos los datos escritos en el LCD
  * @param  None
  * @retval None
  */
void LCD_clean(void){
		LCD_clearL1();
		LCD_clearL2();
}


/**
  * @brief  Se coloca el cursor en el linea y posicion seleccionados
  * @param  line establece la linea en la que se situa el cursor (1 o 2)
  * @param  position establece la posicion del cursor (0-255)
  * @retval None
  */ 
int LCD_setCursor(uint8_t line, uint8_t position){
	if((position > 254) |(line!=1 && line!=2))
		return 1;
	
	if(line==1)
		positionL1 = position;
	else if (line==2)
		positionL2 = position;
	
	return 0;
}


/**
  * @brief  Escritura para la primera linea
  * @param  symbol lo que quieres escribir en la linea
  * @retval 0 si se ha completado correctamente
  * @retval 1 si se han producido errores
  */  
int symbolToLocalBuffer_L1(uint8_t symbol){
	
	uint8_t i, value1, value2;
	uint16_t offset=0;
	
	offset=25*(symbol - ' ');
	
	for (i=0; i<12; i++){
		
		if(positionL1+Arial12x12[offset] > 128U)
			return 1;
		
		value1=Arial12x12[offset+i*2+1];
		value2=Arial12x12[offset+i*2+2];
		
		buffer[i +positionL1]=value1;
		buffer[i+128 +positionL1 ]=value2;
	}
	
	positionL1=positionL1+Arial12x12[offset];
	
	return 0;
}

/**
  * @brief  Escritura para la segunda linea
  * @param  symbol lo que quieres escribir en la linea
  * @retval 0 si se ha completado correctamente
  * @retval 1 si se han producido errores
  */   
int symbolToLocalBuffer_L2(uint8_t symbol){
	
	uint8_t i, value1, value2;
	uint16_t offset=0;
	
	offset=25*(symbol - ' ');
	
	for (i=0; i<12; i++){
		
		if(positionL2+Arial12x12[offset] > 128U)
			return 1;
		
		value1=Arial12x12[offset+i*2+1];
		value2=Arial12x12[offset+i*2+2];
		
		buffer[i +positionL2+ 255]=value1;
		buffer[i+128 +positionL2+ 255 ]=value2;
	}
	
	positionL2=positionL2+Arial12x12[offset];
	
	return 0;
}

/**
  * @brief  Combinación de las funciones anteriores para la escritura
  * @param  libe linea en la que quieres escribir
  * @param  symbol lo que quieres escribir en la linea
  * @retval None
  */  
void symbolToLocalBuffer(uint8_t line,uint8_t symbol){
	if(line==1)
		symbolToLocalBuffer_L1(symbol);
	else if(line==2)
		symbolToLocalBuffer_L2(symbol);
}

/**
  * @brief  Se escribe la cadena dada por par�metro en la linea deseada
  * @param  line Se establece la linea en la que se va a escribir
  * @param  cadena Dato que se quiere escribir en el LCD
  * @retval None
  */ 
int LCD_write_texto(uint8_t line, char* cadena){
	
	if(line == 1){
		LCD_clear(1);
		for(int i = 0; i<strlen(cadena); i++){
			if(positionL1+12 < 127)
				symbolToLocalBuffer_L1(cadena[i]);
		}
	}
	if(line == 2){
		LCD_clear(2);
		for(int i = 0; i<strlen(cadena); i++){
			if(positionL2+12 < 383)
				symbolToLocalBuffer_L2(cadena[i]);
		}
	}
	positionL2 = 256;
	positionL1 = 0;
	LCD_update();
	return 0;
}



 /*----------------------------------------------------------------------------
*                         INICIALIZACION
 *---------------------------------------------------------------------------*/

/**
  * @brief  Inicializacion del módulo LCD
  * @param  None
  * @retval None
  */
void initModuloLCD (void){
	Init_Pins();
	LCD_reset();
	LCD_clear(1);
	LCD_clear(2);
	LCD_init();
}