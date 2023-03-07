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
extern char showtime [25];
extern char showdate [25] ;

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
static void BlinkLed (void *arg);
static void Display  (void *arg);
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
  static uint8_t ip_addr[NET_ADDR_IP6_LEN];
  static char    ip_ascii[40];
  static char    buf[24];
  uint32_t x = 0;

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
  writeLCD("HTTP Server example",1);
//  GLCD_DrawString (x*16U, 4U*24U, "IP4:Waiting for DHCP");

  /* Print Link-local IPv6 address */
//  netIF_GetOption (NET_IF_CLASS_ETH,
//                   netIF_OptionIP6_LinkLocalAddress, ip_addr, sizeof(ip_addr));

//  netIP_ntoa(NET_ADDR_IP6, ip_addr, ip_ascii, sizeof(ip_ascii));

//  sprintf (buf, "IP6:%.16s", ip_ascii);
//  GLCD_DrawString ( x    *16U, 5U*24U, buf);
//  sprintf (buf, "%s", ip_ascii+16);
//  GLCD_DrawString ((x+10U)*16U, 6U*24U, buf);
    netIF_GetOption (NET_IF_CLASS_ETH,
                     netIF_OptionIP4_Address, ip_addr, sizeof(ip_addr));

    netIP_ntoa (NET_ADDR_IP4, ip_addr, ip_ascii, sizeof(ip_ascii));

    sprintf (buf, "IP4:%-16s",ip_ascii);
    writeLCD(buf,2);
  while(1) {
    /* Wait for signal from DHCP */


    /* Retrieve and print IPv4 address */

    
   osThreadFlagsWait (0x01U, osFlagsWaitAny, osWaitForever);
    /* Display user text lines */
    flush_buffer();
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
  }
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
	setTime(7,7,30,1,0,0);
	setDate(6,10,2023,2);
	
  while(1) {
		getTimeStr();
		getDateStr();
	 //printTime();
	 //printDate();
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
  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);

  osThreadExit();
}