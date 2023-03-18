
#include "cmsis_os2.h" 
#include "main.h"

osThreadId_t TID_Th_Sleep;
osTimerId_t Sleep_timer;
uint32_t exec;
static void Th_Sleep (void *arg);
//Sleep funcion declarations
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
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
//  __HAL_RCC_GPIOF_CLK_ENABLE();
//  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = GPIO_PIN_All;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct); 
  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /* Disable GPIOs clock */
	//Used GPIO Ports: A,B,C,D,F,G
//  __HAL_RCC_GPIOA_CLK_DISABLE();
//  __HAL_RCC_GPIOB_CLK_DISABLE();
//  __HAL_RCC_GPIOC_CLK_DISABLE();
//  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
//  __HAL_RCC_GPIOF_CLK_DISABLE();
//  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
  __HAL_RCC_GPIOJ_CLK_DISABLE();
  __HAL_RCC_GPIOK_CLK_DISABLE();

  /* Configure user Button PC13*/
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); //Enable the interruption for the user button
																			//interruption routine @ stm32f4xx_ic.c

  /* Suspend Tick increment to prevent wakeup by Systick interrupt. 
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
	__HAL_RCC_ETH_CLK_ENABLE();
	__HAL_RCC_USB_OTG_FS_CLK_ENABLE();
  
}

//Sleep timer callback:
static void SleepTimer_Callback (void const *arg) {
	//Enter sleep mode
	SleepMode_Measure();
}
//Sleep Timer funcion declarations:
int Init_SleepTimer (void) {
  osStatus_t status;                            // function return status
 
  // Create one-shoot timer
  exec = 1U;
  Sleep_timer = osTimerNew((osTimerFunc_t)&SleepTimer_Callback, osTimerOnce, &exec, NULL);
  if (Sleep_timer != NULL) {  // One-shot timer created
    // start timer with delay 50ms
    status = osTimerStart(Sleep_timer, 50U); 
    if (status != osOK) {
      return -1;
    }
  }
  return NULL;
}
//Sleep init thread declaration
int Init_Th_Sleep (void) {
 
  TID_Th_Sleep = osThreadNew(Th_Sleep, NULL, NULL);
  if (TID_Th_Sleep == NULL) {
    return(-1);
  }
 
  return(0);
}

//Sleep thread
static __NO_RETURN void Th_Sleep(void *arg){
	Init_SleepTimer();
	osTimerStart(Sleep_timer,15000);
  while(1){
		osThreadExit();
  }
}
