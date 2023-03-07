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

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
//#include "Board_Buttons.h"              // ::Board Support:Buttons
//#include "Board_ADC.h"                  // ::Board Support:A/D Converter
//#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD

// Main stack size must be multiple of 8 Bytes
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

//extern char lcd_text[2][20+1];

extern osThreadId_t TID_Display;
//extern osThreadId_t TID_Led;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };

///* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_Th_RTC;

///* Thread declarations */

static void Th_RTC  (void *arg);

__NO_RETURN void app_main (void *arg);
                           
//*******ADC*************
ADC_HandleTypeDef adchandle; //handler definition
/* Read analog inputs */
uint16_t AD_in (uint32_t ch) {
  static int32_t val = 0;
    val = ADC_getValue(&adchandle,10); //value to be scaled in ad.cgi (val*3.3/4096) to voltage
  return ((uint16_t)val);
}

//**********************************
///* Read digital inputs */
//uint8_t get_button (void) {
//  return ((uint8_t)Buttons_GetState ());
//}


///*----------------------------------------------------------------------------
//  Thread 'RTC': Set and get RTC date and time periodically
// *---------------------------------------------------------------------------*/
static __NO_RETURN void Th_RTC (void *arg) {
	//LCD init to display on screen
	LCD_reset();
  osDelay(50);
  LCD_init();
	
	RTC_init();
	setTime(7,7,30,1,0,0);
	setDate(6,10,2023,2);
	
  while(1) {

	 printTime();
	 printDate();
	 //Set clock refresh rate in ms
    osDelay (1000); //Every 3 mins
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

  LED_Initialize();
//  Buttons_Initialize();
//  ADC_Initialize();
  	
  ADC1_pins_F429ZI_config(); //specific PINS configuration
  
  ADC_Init_Single_Conversion(&adchandle , ADC1);
  
  netInitialize ();

	TID_Th_RTC  = osThreadNew (Th_RTC,  NULL, NULL);

  osThreadExit();
}
