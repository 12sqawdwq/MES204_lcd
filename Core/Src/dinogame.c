#include "../Inc/dinogame.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdbool.h>

// ======================================
// 1. 游戏状态变量定义
// ======================================
volatile uint8_t grass_status[COL] = {0};
volatile int8_t jump_status = 0;
volatile bool jump_order = false;
volatile bool game_start = false;
volatile uint16_t score = 0;

uint8_t grass_old_status[COL] = {0};
int8_t jump_old_status = 0;

// ======================================
// 2. 游戏初始化函数
// ======================================

void Game_Status_Init(void)
{
    // 重置所有状态变量
    for(int i = 0; i < COL; i++) {
        grass_status[i] = 0;
        grass_old_status[i] = 255;
    }
    jump_status = 0;
    jump_old_status = -1;
    jump_order = false;
    game_start = true;
    score = 0;
}

// ======================================
// 3. 辅助函数：获取障碍物年龄 (get_grass_age)
// ======================================

/**
 * @brief 查找最右侧障碍物的位置，用于控制新障碍物生成间隔。
 * @retval 最右侧障碍物距离 COL 的距离 + 1。
 */
int get_grass_age(void)
{
    for (int i = COL - 1; i >= 0; i--)
    {
        if (grass_status[i] != 0)
        {
            // COL - i 就是障碍物距离左侧（Dino）的距离。
            return COL - i;
        }
    }
    return COL + 1; // 如果没有障碍物，返回 COL + 1
}

// ======================================
// 4. 核心函数：绘制函数 (基于状态数组)
// ======================================

/**
 * @brief  显示单个障碍物列的状态。
 * @param  lcd: LCD 句柄
 * @param  index: 要绘制的列索引 (0-15)
 */
void display_grass_status(Lcd_HandleTypeDef *lcd, uint8_t index)
{
    // 障碍物在第二行 (row=1)
    Lcd_cursor(lcd, 1, index);

    if (grass_status[index] == 0)
    {
        lcd_write_data(lcd, ' '); // 状态 0 是空格
    }
    else if (grass_status[index] >= 1 && grass_status[index] <= 4)
    {
        // 状态 1-4 对应 CGRAM 0x01-0x03
        if (grass_status[index] == 4) {
             lcd_write_data(lcd, CHAR_GRASS_F3); // 0x03
        } else {
             // 状态 1 -> 0x01, 状态 2 -> 0x02, 状态 3 -> 0x03
             lcd_write_data(lcd, grass_status[index]);
        }
    }
}


/**
 * @brief 显示恐龙跳跃状态。
 * 恐龙始终位于第0列 (index 0)
 */
void display_jump_status(Lcd_HandleTypeDef *lcd)
{
    uint8_t top_char = ' '; // 第一行 (row=0) 的字符
    uint8_t bottom_char = ' '; // 第二行 (row=1) 的字符

    // jump_status 范围 0-15
    if (jump_status == 0)
    {
        // 站立在地面： Row 1 显示 CHAR_DINO_STAND (0x04)
        top_char = ' ';
        bottom_char = CHAR_DINO_STAND; // 0x04
    }
    else if (jump_status >= 1 && jump_status <= 4)
    {
        // 上升起始 (跑/跳)
        top_char = CHAR_DINO_JUMP; // 0x05
        bottom_char = CHAR_DINO_RUN1; // 0x06 (腿部动画，以保持跑步感)
    }
    else if (jump_status >= 5 && jump_status <= 11)
    {
        // 跳跃最高点 (头部在 Row 0)
        top_char = CHAR_DINO_JUMP; // 0x05
        bottom_char = ' ';
    }
    else // jump_status 12 ~ 15
    {
        // 落地过程 (头部下降，脚部还未接触)
        top_char = CHAR_DINO_STAND; // 0x04 (头部从空中下来)
        bottom_char = ' ';
    }

    // 绘制第一行 (Row 0, Col 0)
    Lcd_cursor(lcd, 0, 0);
    lcd_write_data(lcd, top_char);

    // 绘制第二行 (Row 1, Col 0)
    Lcd_cursor(lcd, 1, 0);
    lcd_write_data(lcd, bottom_char);
}


// ======================================
// 5. 核心游戏循环函数
// ======================================

/**
 * @brief  核心游戏循环，在定时器中断中周期性调用。
 * @param  lcd: LCD 句柄
 * @retval None
 */
void Game_Core_Loop(Lcd_HandleTypeDef *lcd)
{
    if (!game_start)
    {
        // 游戏未开始时不执行动态逻辑
        return;
    }

    // --- 1. 障碍物生成 ---
    srand(HAL_GetTick());
    // 只有当最右侧障碍物远离恐龙（距离 > 8）时，才有几率生成新的障碍物
    if (get_grass_age() > 8 && (rand() % 10) > 8)
    {
        grass_status[COL - 1] = 1; // 新草地从最右侧开始
    }

    // --- 2. 状态更新 ---
    Game_Update_Grass();
    Game_Update_Jump(); // !!! 启用跳跃状态更新 !!!

    // --- 3. 绘图与差异化刷新 ---
    // 3.1. 障碍物刷新
    for (int i = 0; i < COL; i++)
    {
        if (grass_old_status[i] != grass_status[i])
        {
            display_grass_status(lcd, i);
            grass_old_status[i] = grass_status[i];
        }
    }

    // 3.2. 恐龙跳跃刷新 (始终在第 0 列)
    if (jump_status != jump_old_status)
    {
        display_jump_status(lcd);
        jump_old_status = jump_status;
    }

    // --- 4. 得分刷新 ---
    static uint16_t score_old = 0; // 在 core loop 中跟踪旧得分

    if (score != score_old)
    {
        // 绘制得分 (在第一行，例如第 16 列 - 3 个数字位)
        Lcd_cursor(lcd, 0, COL - 3); // 假设最大 3 位数得分，从第 13 列开始 (16-3)
        Lcd_int(lcd, score);        // 使用 Lcd_int 打印分数
        score_old = score;
    }

    // *** 5. 碰撞检测 (如果需要，可在此处添加逻辑) ***
}

/**
 * @brief  移植自 Python 脚本的复杂障碍物更新逻辑。
 * 负责障碍物移动、帧动画切换和得分计算。
 */
void Game_Update_Grass(void)
{
    int i = 0;
    while (i < COL)
    {
        // 状态 0: 空白，继续检查下一个
        if (grass_status[i] == 0)
        {
            i = i + 1;
        }
        // 状态 1: 变为 2，并向右侧 (i+1) 传递状态 4，然后跳过 i+1
        else if (grass_status[i] == 1)
        {
            if (i == COL - 1)
            {
                // 如果在最右侧，只更新当前状态，不传递
                grass_status[i] = 2;
                return; // 提前退出循环 (Python 逻辑)
            }
            else
            {
                grass_status[i] = 2;
                grass_status[i + 1] = 4;
                i = i + 2; // 跳过下一格
            }
        }
        // 状态 2: 变为 3，并向左侧 (i-1) 传递状态 1，向右侧 (i+1) 清空 0
        else if (grass_status[i] == 2)
        {
            grass_status[i] = 3;
            if (i != 0)
            {
                grass_status[i - 1] = 1;
            }
            if (i != COL - 1)
            {
                grass_status[i + 1] = 0;
            }
            i = i + 1;
        }
        // 状态 3: 变为 4，继续检查下一个
        else if (grass_status[i] == 3)
        {
            grass_status[i] = 4;
            i = i + 1;
        }
        // 状态 4: 变为 0 (清空)，并在最左侧 (i=0) 时计分
        else // grass_status[i] == 4
        {
            grass_status[i] = 0;
            if (i == 0)
            {
                score = score + 1;  // 障碍物成功通过恐龙位置，得分 + 1
            }
            i = i + 1;
        }
    }
}

/**
 * @brief  处理恐龙的跳跃状态和动画。
 * 跳跃过程使用 16 个状态 (0-15) 来表示一个完整的跳跃周期。
 */
void Game_Update_Jump(void)
{
    // 1. 处理跳跃请求
    if (jump_order == true && jump_status == 0)
    {
        // 只有在地面 (jump_status == 0) 且有跳跃请求时才开始跳跃
        jump_status = 1;
        jump_order = false; // 清除请求
    }

    // 2. 驱动跳跃动画
    if (jump_status > 0)
    {
        jump_status++;
    }

    // 3. 落地检测
    if (jump_status > 15)
    {
        jump_status = 0; // 超过 15 帧，回到地面状态
    }
}