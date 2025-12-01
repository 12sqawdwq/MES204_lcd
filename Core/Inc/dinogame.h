#ifndef INC_DINOGAME_H_
#define INC_DINOGAME_H_

#include "main.h" // 包含 STM32 HAL 定义
#include "lcd.h"  // 包含 LCD 结构体定义
#include <stdbool.h>
#include <stdint.h>

// 宏定义 LCD 列数 (16x2 LCD)
#define COL 16

// CGRAM 字符地址定义 (与 lcd.c 中 custom_chars 数组的顺序对应)
#define CHAR_GRASS_F0     0x00
#define CHAR_GRASS_F1     0x01
#define CHAR_GRASS_F2     0x02
#define CHAR_GRASS_F3     0x03
#define CHAR_DINO_A       0x04
#define CHAR_DINO_B       0x05
#define CHAR_DINO_JUMP_TOP       0x06
#define CHAR_DINO_JUMP_BOTTOM    0x07


// ======================================
// 外部变量声明 (在 dinogame.c 中定义)
// ======================================
extern volatile uint8_t grass_status[COL];
extern volatile int8_t jump_status; // 恐龙跳跃状态：0 (地面) 到 15 (落地)
extern volatile bool jump_order;    // 跳跃请求标志 (由按钮中断设置)
extern volatile bool game_start;    // 游戏开始标志
extern volatile uint16_t score;

// ======================================
// 函数原型
// ======================================
void Game_Status_Init(void);
void Game_Core_Loop(Lcd_HandleTypeDef *lcd);
void Game_Update_Grass(void);
// 启用跳跃状态更新函数
void Game_Update_Jump(void);

#endif /* INC_DINOGAME_H_ */