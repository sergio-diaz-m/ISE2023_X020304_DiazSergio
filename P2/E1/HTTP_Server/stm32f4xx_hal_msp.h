#ifndef __STM32F4XXHALMSP_HS
#define __STM32F4XXHALMSP_H

#include <stdint.h>
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);
#endif //__STM32F4XXHALMSP_H