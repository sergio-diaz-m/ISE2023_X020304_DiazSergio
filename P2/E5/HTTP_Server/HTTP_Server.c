/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Net_Config_SNTP_Client.h"
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
extern char showtime [25];
extern char showdate [25] ;

//extern char lcd_text[2][20+1];

extern osThreadId_t TID_Display;
//extern osThreadId_t TID_Led;

bool LEDrun;
int sntp_sel;

char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };

char alarm_h [2]="00" ;
char alarm_m [2]="00" ;
char alarm_s [2]="00" ;
													
													 
static uint8_t ip_addr[NET_ADDR_IP6_LEN];
static char    ip_ascii[40];
static char    buf[24];
													 
///* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_Th_RTC;
osThreadId_t TID_Th_SNTP;

///* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);
static void Th_RTC  (void *arg);
static void Th_SNTP  (void *arg);

__NO_RETURN void app_main (void *arg);

													 
//*******USER BUTTON*****

//GPIO TYPEDEF
static GPIO_InitTypeDef GPIO_InitStruct={0};
//USER BUTTON INIT
static void config_pin_usrbtn()
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); //Enable the interruption for the user button
																			//interruption routine @ stm32f4xx_ic.c
}
//**********************

//******* Blink LEDs SNTP **************
//Blink LEDs when SNTP server 
//updates local time
//**************************************

void SNTP_ledsBlink(void){
	uint8_t i;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
	for(i=0; i<8; i++){
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
		osDelay(250);
	}
}
//**************************************

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

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
//    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Display (void *arg) {
  (void)arg;
  LCD_reset();
  osDelay(50);
  LCD_init();
//  GLCD_Initialize         ();
//  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
//  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
//  GLCD_ClearScreen        ();
//  GLCD_SetFont            (&GLCD_Font_16x24);
//  GLCD_DrawString         (x*16U, 1U*24U, "       MDK-MW       ");
//  GLCD_DrawString         (x*16U, 2U*24U, "HTTP Server example ");
//  GLCD_DrawString (x*16U, 4U*24U, "IP4:Waiting for DHCP");

  /* Print Link-local IPv6 address */
//  netIF_GetOption (NET_IF_CLASS_ETH,
//                   netIF_OptionIP6_LinkLocalAddress, ip_addr, sizeof(ip_addr));

//  netIP_ntoa(NET_ADDR_IP6, ip_addr, ip_ascii, sizeof(ip_ascii));

//  sprintf (buf, "IP6:%.16s", ip_ascii);
//  GLCD_DrawString ( x    *16U, 5U*24U, buf);
//  sprintf (buf, "%s", ip_ascii+16);
//  GLCD_DrawString ((x+10U)*16U, 6U*24U, buf);


  while(1) {
    /* Wait for signal from DHCP */


    /* Retrieve and print IPv4 address */

    
   osThreadFlagsWait (0x01U, osFlagsWaitAny, osWaitForever);
    /* Display user text lines */
    osDelay(50);
    sprintf (buf, "%-20s", lcd_text[0]);
    writeLCD(buf,1);
    
    sprintf (buf, "%-20s", lcd_text[1]);
    writeLCD(buf,2);
  }
}

///*----------------------------------------------------------------------------
//  Thread 'BlinkLed': Blink the LEDs on an eval board
// *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  const uint8_t led_val[5] = { 0x00,0x01,0x02,0x04,0x10 };
  (void)arg;
  uint32_t cnt=0U;
  LEDrun = true;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      LED_Toggle (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
		if(osThreadFlagsWait(0x04,osFlagsWaitAny,0)==0x04){
			SNTP_ledsBlink();
		}
  }
}

///*----------------------------------------------------------------------------
//  Thread 'SNTP': Gets the current time from a SNTP server every 60 minutes
// *---------------------------------------------------------------------------*/


struct tm time_now;

static void time_callback (uint32_t seconds, uint32_t seconds_fraction);


void get_time (void) {
	const NET_ADDR4 ntp_server1 = { NET_ADDR_IP4, 0, 130, 206, 3, 166 }; // hora.rediris.es
	const NET_ADDR4 ntp_server2 = { NET_ADDR_IP4, 0, 150, 214, 94, 5 }; // hora.roa.es
	
	//Server select 2
	if(sntp_sel){
		if (netSNTPc_GetTime ((NET_ADDR*)&ntp_server2, time_callback) == netOK) {
			//printf ("SNTP request sent.\n");
		}
		else {
    //printf ("SNTP not ready or bad parameters.\n");
		}
  }
	//Server select 1
	else{
		if (netSNTPc_GetTime ((NET_ADDR*)&ntp_server1, time_callback) == netOK) {
			//printf ("SNTP request sent.\n");
		}
		else {
    //printf ("SNTP not ready or bad parameters.\n");
		}
	}
}
 
static void time_callback (uint32_t seconds, uint32_t seconds_fraction) {
  if (seconds == 0) {
    //printf ("Server not responding or bad response.\n");
  }
  else {
    //printf ("%d seconds elapsed since 1.1.1970\n", seconds);
	  time_now= *localtime(&seconds);
		RTC_init();
		setTime(time_now.tm_hour+1,time_now.tm_min,time_now.tm_sec,0,time_now.tm_isdst,0); //GMT+1
		setDate(time_now.tm_mday,time_now.tm_mon+1,time_now.tm_year+1900,time_now.tm_wday);
  }
}

static __NO_RETURN void Th_SNTP (void *arg) {
	while(1){
		get_time();
		osThreadFlagsSet (TID_Led, 0x04);
		if(osThreadFlagsWait(0x08,osFlagsWaitAny,180000)==0x08){
			//Do nothing
		}
	}
}
//RTC variable definitions
extern RTC_HandleTypeDef hrtc;
extern RTC_TypeDef rtc; 
RTC_AlarmTypeDef sAlarm;

void RTC_Alarm_IRQHandler(void) {
  HAL_RTC_AlarmIRQHandler(&hrtc);
}
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){

	
}

///*----------------------------------------------------------------------------
//  Thread 'RTC': Set and get RTC date and time periodically
// *---------------------------------------------------------------------------*/
static __NO_RETURN void Th_RTC (void *arg) {
	//LCD init to display on screen


	
	LCD_reset();
  osDelay(50);
  LCD_init();
	
	
	RTC_init();
	
//	setTime(7,7,30,1,0,0);
//	setDate(6,10,2023,2);
	
  while(1) {
	 getTimeStr();
	 getDateStr();
		
	 printTime();
	 printDate();
	 //Set clock refresh rate in ms
   if(osThreadFlagsWait(0x10,osFlagsWaitAny,250)==0x10){
//		 test=((alarm_h[0]-0x30)*10 +(alarm_h[1]-0x30));
		sAlarm.AlarmTime.Hours = ((alarm_h[0]-0x30)*10 +(alarm_h[1]-0x30)) ;
		sAlarm.AlarmTime.Minutes = (alarm_m[0]-0x30)*10 +(alarm_m[1]-0x30);
		sAlarm.AlarmTime.Seconds = (alarm_s[0]-0x30)*10 +(alarm_s[1]-0x30);
		sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
		sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
		sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
		sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
		sAlarm.AlarmDateWeekDay = 1;
		sAlarm.Alarm = RTC_ALARM_A;
		HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);
		HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	 }
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
	
	config_pin_usrbtn();
	
  netInitialize ();
	TID_Display = osThreadNew (Display,  NULL, NULL);
	
	//*********Inital screen print**********************
	writeLCD("HTTP Server example",1);
	netIF_GetOption (NET_IF_CLASS_ETH,
									 netIF_OptionIP4_Address, ip_addr, sizeof(ip_addr));

	netIP_ntoa (NET_ADDR_IP4, ip_addr, ip_ascii, sizeof(ip_ascii));

	sprintf (buf, "IP4:%-16s",ip_ascii);
	writeLCD(buf,2);
	
	osDelay(5000);
	flush_buffer();
	//*********End of initial screen********************
	
  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
	
	TID_Th_RTC  = osThreadNew (Th_RTC,  NULL, NULL);
	TID_Th_SNTP  = osThreadNew (Th_SNTP,  NULL, NULL);
  osThreadExit();
}
