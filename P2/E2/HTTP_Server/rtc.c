/**
  ******************************************************************************
  * @file    RTC/RTC_Calendar/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
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
#include <stdio.h>

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @defgroup RTC_Calendar
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

 //*******RTC init*******
static RTC_TimeTypeDef timeRTC;
static RTC_DateTypeDef dateRTC;
static RTC_TimeTypeDef currentTime;
static RTC_DateTypeDef currentDate;
static RTC_HandleTypeDef hrtc;
static RTC_TypeDef rtc; 

  
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
   
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    
  }
  
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
}

/**
  * @brief RTC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_RTC_DISABLE();

  /*##-2- Disables the PWR Clock and Disables access to the backup domain ###################################*/
  HAL_PWR_DisableBkUpAccess();
  __HAL_RCC_PWR_CLK_DISABLE();
  
}

//***** SET TIME****
void setTime(uint8_t Hours, uint8_t Minutes, uint8_t Seconds, uint8_t AM_PM, uint8_t DST, uint8_t set){
  timeRTC.Seconds=Seconds;
  timeRTC.Minutes=Minutes;
  timeRTC.Hours=Hours;

  if(AM_PM==1){ //AM 0 PM 1
    timeRTC.TimeFormat = RTC_HOURFORMAT12_PM;
  }
  else{
    timeRTC.TimeFormat = RTC_HOURFORMAT12_AM;
  }

	switch(DST){
		case 0:
			timeRTC.DayLightSaving=RTC_DAYLIGHTSAVING_NONE; //no DST
			break;
		case 1:
			timeRTC.DayLightSaving=RTC_DAYLIGHTSAVING_ADD1H; //Add 1h
			break;
		case 2:
			timeRTC.DayLightSaving=RTC_DAYLIGHTSAVING_SUB1H; //Subtract 1h
			break;
		default:
			timeRTC.DayLightSaving=RTC_DAYLIGHTSAVING_NONE; //no DST (default)
			break; 
	}

  if(set==1){ //Store operation set 1, reset 0
      timeRTC.StoreOperation = RTC_STOREOPERATION_SET;
  }
  else{
      timeRTC.StoreOperation = RTC_STOREOPERATION_RESET;
  }
  
  if (HAL_RTC_SetTime(&hrtc, &timeRTC, RTC_FORMAT_BCD) != HAL_OK) {
    //Error handle
  }
}


//***** SET DATE****
void setDate(uint8_t day, uint8_t month, uint16_t year, uint8_t weekday){
	dateRTC.WeekDay = weekday;
	dateRTC.Date = day;
  dateRTC.Month = month;
  dateRTC.Year = year-2000;
	
	if (HAL_RTC_SetDate(&hrtc, &dateRTC, RTC_FORMAT_BCD) != HAL_OK) {
    //Error handle
  }
}

//***** GET TIME ****
void getTime(void){
	if (HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN) != HAL_OK) {
    //Error handle
   }
}

//***** GET DATE ****
void getDate(void){
	if (HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN) != HAL_OK) {
		//Error handle
   }
}

//***** PRINT TIME (upper row) ****
void printTime(void){
 //Display on LCDs
	static char showtime[25];
  getTime();
	sprintf(showtime, "     %02d:%02d:%02d", currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
	writeLCD(showtime,1);
}
//***** PRINT DATE (bottom row) ****
void printDate(void){
	static char showdate[25];
  getDate();
	/* Display date Format : mm-dd-yy */
	sprintf(showdate, "    %02d-%02d-%2d", currentDate.Date, currentDate.Month, 2000 + currentDate.Year);
	writeLCD(showdate,2);
}
/**
  * @}
  */

/**
  * @}
  */
