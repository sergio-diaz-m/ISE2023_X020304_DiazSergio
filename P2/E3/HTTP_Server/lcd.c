

#include "Driver_SPI.h"
#include "main.h"
#include "Arial12x12.h"
#include <string.h>
#include <stdio.h>

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI *SPIdrv = &Driver_SPI1;
GPIO_InitTypeDef GPIO_InitStruct = {0};
static TIM_HandleTypeDef tim7;


unsigned char buffer[512] = "0";
static uint8_t positionL1;
static uint8_t positionL2;


void mySPI_callback(uint32_t event)
{
    switch (event)
    {
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
      
        break;
    case ARM_SPI_EVENT_DATA_LOST:
        /*  Occurs in slave mode when data is requested/sent by master
            but send/receive/transfer operation has not been started
            and indicates that data is lost. Occurs also in master mode
            when driver cannot transfer data fast enough. */
        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    case ARM_SPI_EVENT_MODE_FAULT:
        /*  Occurs in master mode when Slave Select is deactivated and
            indicates Master Mode Fault. */
        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    }
}

static void delay(uint16_t time_us)
{
   __HAL_RCC_TIM7_CLK_ENABLE();
   tim7.Instance=TIM7;
   tim7.Init.Prescaler=83;
   tim7.Init.Period=999;
   
   HAL_TIM_Base_Init(&tim7);
   HAL_TIM_Base_Start(&tim7);
   
    TIM7->CNT = 0;
    while ( TIM7->CNT < time_us);

   HAL_TIM_Base_DeInit(&tim7);	//Timer stop and deinit
   HAL_TIM_Base_Stop(&tim7);
}

void LCD_reset (void)
{
  SPIdrv -> Initialize (mySPI_callback);
  SPIdrv -> PowerControl(ARM_POWER_FULL);
  SPIdrv -> Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 2000000); // segundo argumento bus speed


  // Inicializaci?n pines GPIO
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pin   = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // PA6  --> RESET

  GPIO_InitStruct.Pin   = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct); // PF13 --> A0

  GPIO_InitStruct.Pin   = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct); // PD14 --> /CS

  
  
  //pulso de reset
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delay(5); // 1 us como m�nimo para captar la se�al de reset
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  delay(5); // 1 us como m�nimo para que haga el reset
  delay(500);
  
}

void LCD_wr_data(unsigned char data)
{
	// SPIdrv 	-> 	Control	(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); // /CS --> 0
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET); // A0 --> 1 (datos)
	SPIdrv->Send(&data, sizeof(data));
	
	while (SPIdrv->GetStatus().busy); // espera bloqueante

	// SPIdrv 	-> 	Control	(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); // /CS --> 1
}

void LCD_wr_cmd(unsigned char data)
{
  
	// SPIdrv 	-> 	Control	(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET); // /CS --> 0
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET); // A0 --> 0 (comandos)
	SPIdrv->Send(&data, sizeof(data));
	
	while (SPIdrv->GetStatus().busy); // espera bloqueante
	
	// SPIdrv 	-> 	Control	(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); // /CS --> 1
}

void LCD_init (void)
{
  LCD_wr_cmd(0xae); //Display off
  LCD_wr_cmd(0xa2); // Vpol 1/9
  LCD_wr_cmd(0xa0); // Normal RAM dir
  LCD_wr_cmd(0xc8); // Normal scan COM
  LCD_wr_cmd(0x22); // Relacion de resistencias internas a 2
  LCD_wr_cmd(0x2f); // Power ON
  LCD_wr_cmd(0x40); // Set the cursor to the first line (0)
  LCD_wr_cmd(0xaf); // Display ON
  LCD_wr_cmd(0x81); // set contrast
  LCD_wr_cmd(0x10); // contrast value (Electronic volume value register??)
  LCD_wr_cmd(0xa4); // display all points normal
  LCD_wr_cmd(0xa6); // LCD Display normal
  
}
void LCD_update(void)
{
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

void symbolToLocalBuffer (uint8_t line, uint8_t symbol){
  uint8_t i, value1, value2;
  uint16_t offset=0;
  uint8_t page1;
  uint8_t page2;
  uint8_t *position;
  
  if (symbol > 186)
    return;
  
  if (line == 2)
    line++;
  
  page1 = line - 1;
  page2 = line;
  
  if (line == 1)
    position = &positionL1;
  else
    position = &positionL2;
  
  offset = 25*(symbol - ' ');
  
  for (i=0; i<12; i++)
  {
    value1=Arial12x12[offset+i*2+1];
    value2=Arial12x12[offset+i*2+2];
    
    buffer[i + (page1*128) + *position] = value1;
    buffer[i + (page2*128) + *position] = value2;
  }
  *position += Arial12x12[offset];
}

void writeLCD (char * str, int line) {
  if (line == 1)
    positionL1 = 0;
  else
    positionL2 = 0;
  while(*str)
  {
    symbolToLocalBuffer(line, *str);
    LCD_update();
    str++;
  }
}

void	*ft_memset(void *str, int c, size_t n)
{
	size_t		i;

	i = 0;
	while (i < n)
	{
		*((unsigned char *)str + i) = c;
		i++;
	}
	return (str);
}

void flush_buffer (void)
{
  ft_memset(buffer, 0, sizeof(buffer));
}
void flush_half_buffer (void)
{
  ft_memset(buffer, 0, sizeof(buffer)/2);
}
