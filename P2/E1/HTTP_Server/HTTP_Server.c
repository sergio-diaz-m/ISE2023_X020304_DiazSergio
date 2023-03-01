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

#include "stm32f4xx_hal_msp.h"
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

//*******RTC*******
RTC_TimeTypeDef timeRTC;
RTC_DateTypeDef dateRTC;
RTC_TimeTypeDef currentTime;
RTC_DateTypeDef currentDate;
RTC_HandleTypeDef hrtc;
RTC_TypeDef rtc; 

	
void RTC_init(void){
	
	HAL_RTC_MspInit(&hrtc);
	
  hrtc.Instance = RTC; 
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	__HAL_RTC_RESET_HANDLE_STATE(&hrtc);
  if (HAL_RTC_Init(&hrtc) != HAL_OK) {
    // Handle error return -1
  }
	
}


///*----------------------------------------------------------------------------
//  Thread 'RTC': Set and get RTC date and time periodically
// *---------------------------------------------------------------------------*/
static __NO_RETURN void Th_RTC (void *arg) {
	LCD_reset();
  osDelay(50);
  LCD_init();
	
	RTC_init();
	
	//Set time 00:00:00
	static char showtime[25];
	static char showdate[25];
	
	timeRTC.Seconds=0x10;
	timeRTC.Minutes=0x10;
	timeRTC.Hours=0x10;
	timeRTC.TimeFormat = RTC_HOURFORMAT12_AM;
	timeRTC.DayLightSaving=RTC_DAYLIGHTSAVING_NONE; //no DST
  timeRTC.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &timeRTC, RTC_FORMAT_BCD) != HAL_OK) {
    //Error handle
  }
	//Set date 01 jan MON
	dateRTC.WeekDay = RTC_WEEKDAY_TUESDAY;
  dateRTC.Month = RTC_MONTH_JUNE;
  dateRTC.Date = 0x10;
  dateRTC.Year = 0x0;
  if (HAL_RTC_SetDate(&hrtc, &dateRTC, RTC_FORMAT_BCD) != HAL_OK) {
    //Error handle
  }
  while(1) {

   if (HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN) != HAL_OK) {
    //Error handle
   }
	 if (HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN) != HAL_OK) {
		//Error handle
   }
	  
	  sprintf(showtime, "     %02d:%02d:%02d", currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
		writeLCD(showtime,1);
		/* Display date Format : mm-dd-yy */
		sprintf(showdate, "    %02d-%02d-%2d", currentDate.Date, currentDate.Month, 2000 + currentDate.Year);
		writeLCD(showdate,2);
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
