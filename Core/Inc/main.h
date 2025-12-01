/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"

#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
  void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Button_Pin GPIO_PIN_13
#define Button_GPIO_Port GPIOC
#define Button_EXTI_IRQn EXTI15_10_IRQn
#define LED_LD2_Pin GPIO_PIN_5
#define LED_LD2_GPIO_Port GPIOA
#define LCD_D4_Pin GPIO_PIN_6
#define LCD_D4_GPIO_Port GPIOC
#define LCD_D5_Pin GPIO_PIN_7
#define LCD_D5_GPIO_Port GPIOC
#define LCD_D6_Pin GPIO_PIN_8
#define LCD_D6_GPIO_Port GPIOC
#define LCD_D7_Pin GPIO_PIN_9
#define LCD_D7_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_8
#define LCD_RS_GPIO_Port GPIOB
#define LCD_EN_Pin GPIO_PIN_9
#define LCD_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
  typedef enum {
    STATE_CLEAR,   // 清屏状态
    STATE_DISPLAY  // 显示状态
  } Lcd_State_t;

  extern volatile Lcd_State_t lcd_state;
  extern Lcd_HandleTypeDef lcd; /* 全局句柄 */

  // ======================================
  // 1. 游戏参数 (来自 Python 脚本)
  // ======================================
#define COL 16             // LCD 列数
#define JUMP_CYCLE 16      // 跳跃动画总帧数

  // ======================================
  // 2. CGRAM 地址宏 (与 lcd.c 中的 char_ 数组对应)
  // ======================================
#define CHAR_EMPTY       0x00 // CGRAM 0x00
#define CHAR_GRASS_F1    0x01 // CGRAM 0x01
#define CHAR_GRASS_F2    0x02 // CGRAM 0x02
#define CHAR_GRASS_F3    0x03 // CGRAM 0x03

#define CHAR_DINO_STAND  0x04 // CGRAM 0x04 (站立/跳跃中间)
#define CHAR_DINO_JUMP   0x05 // CGRAM 0x05 (跳跃顶部)
#define CHAR_DINO_RUN1   0x06 // CGRAM 0x06 (跑腿帧 1)
#define CHAR_DINO_RUN2   0x07 // CGRAM 0x07 (跑腿帧 2)


  // ======================================
  // 4. 游戏逻辑函数声明
  // ======================================
  void Game_Status_Init(void);
  void Game_Update_Grass(void);
  void Game_Update_Jump(void);
  void Game_Draw_Frame(Lcd_HandleTypeDef *lcd);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
