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

    uint8_t char_code;

    // 状态 0 对应 空格
    if (grass_status[index] == 0)
    {
        char_code = ' ';
    }
    // 状态 1 对应 CGRAM 0x00 (F0)
    else if (grass_status[index] == 1)
    {
        char_code = CHAR_GRASS_F0; // 0x00
    }
    // 状态 2 对应 CGRAM 0x01 (F1)
    else if (grass_status[index] == 2)
    {
        char_code = CHAR_GRASS_F1; // 0x01
    }
    // 状态 3 对应 CGRAM 0x02 (F2)
    else if (grass_status[index] == 3)
    {
        char_code = CHAR_GRASS_F2; // 0x02
    }
    // 状态 4 对应 CGRAM 0x03 (F3)
    else // grass_status[index] == 4
    {
        char_code = CHAR_GRASS_F3; // 0x03
    }

    lcd_write_data(lcd, char_code);
}

/**
 * @brief 显示恐龙跳跃状态。
 * 恐龙始终位于第0列 (index 0)
 */
void display_jump_status(Lcd_HandleTypeDef *lcd)
{
    // 恐龙始终在第 0 列绘制
    extern int8_t jump_old_status; // 需要在 dinogame.c 顶部声明

    // 绘制第一行 (row=0)
    Lcd_cursor(lcd, 0, 0);

    if (jump_status == 0)
    {
        // 状态 0: 站立/跑动动画。
        // Row 0: 空格
        lcd_write_data(lcd, ' ');
    }
    else if (jump_status == 2)
    {
        // 状态 2: 低位跳跃。 Row 0: 0x05
        lcd_write_data(lcd, CHAR_DINO_B); // 假设 0x05 对应 CHAR_DINO_B
    }
    else if (jump_status == 4)
    {
        // 状态 4: 中位跳跃。 Row 0: 0x04
        lcd_write_data(lcd, CHAR_DINO_A); // 假设 0x04 对应 CHAR_DINO_A
    }
    else if (jump_status == 8)
    {
        // 状态 8: 最高点。 Row 0: 0x07
        lcd_write_data(lcd, CHAR_DINO_JUMP_BOTTOM); // 假设 0x07 对应跳跃最高点帧
    }
    else if (jump_status == 12)
    {
        // 状态 12: 下落。 Row 0: 0x04
        lcd_write_data(lcd, CHAR_DINO_A);
    }
    else if (jump_status == 14)
    {
        // 状态 14: 落地前。 Row 0: 0x05
        lcd_write_data(lcd, CHAR_DINO_B);
    }
    else
    {
        // 其它中间帧，保持 Row 0 空格或不做修改，以简化。
        lcd_write_data(lcd, ' ');
    }


    // 绘制第二行 (row=1)
    Lcd_cursor(lcd, 1, 0);

    if (jump_status == 0)
    {
        // 状态 0: 站立/跑动动画。 Row 1: 0x04 (站立)
        // 注意：原 Python 逻辑中，0x04 是站立，但为了跑动动画，我们使用 0x04 和 0x05 交替。
        // 为了简化，这里我们只显示 0x04
        lcd_write_data(lcd, CHAR_DINO_A);
    }
    else if (jump_status == 2)
    {
        // 状态 2: 低位跳跃。 Row 1: 0x06
        lcd_write_data(lcd, CHAR_DINO_JUMP_TOP); // 假设 0x06 对应跳跃底部帧
    }
    else if (jump_status == 4)
    {
        // 状态 4: 中位跳跃。 Row 1: 空格
        lcd_write_data(lcd, ' ');
    }
    else if (jump_status == 14)
    {
        // 状态 14: 落地前。 Row 1: 0x06
        lcd_write_data(lcd, CHAR_DINO_JUMP_TOP);
    }
    else
    {
        // 其它帧，Row 1 为空格
        lcd_write_data(lcd, ' ');
    }

    // 如果是状态 0，但 jump_old_status 是 JUMP_CYCLE-1 (落地瞬间)
    if (jump_status == 0 && jump_old_status == JUMP_CYCLE - 1)
    {
        // 理论上应该处理落地瞬间的清除，但我们的逻辑在 jump_status=0 时已经绘制了 CHAR_DINO_A。
        // 为了严格遵循 Python 逻辑，我们可以在 Row 0, 0 处再清除一次（虽然看起来是多余的）。
        // LCD.print_num(0, 0, ord(" "))
        // LCD.print_num(0, 1, 4)
    }

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

    // // --- 1. 障碍物生成 ---
    // srand(HAL_GetTick());
    // // 只有当最右侧障碍物远离恐龙（距离 > 8）时，才有几率生成新的障碍物
    // if (get_grass_age() > 8 && (rand() % 10) > 8)
    // {
    //     grass_status[COL - 1] = 1; // 新草地从最右侧开始
    // }

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
/**
 * @brief  忠实移植自 Python 脚本的复杂障碍物更新逻辑 (原地修改 + 索引跳跃)。
 * 状态循环: 0 -> 1 -> 2 -> 3 -> 4 -> 0
 */
void Game_Update_Grass(void)
{
    // --- 1. 碰撞检测 ---
    // 恐龙在地面 (jump_status == 0) 且障碍物在第 0 列 (i=0)
    if (grass_status[0] != 0 && jump_status == 0)
    {
        game_start = false; // 碰撞发生，游戏结束
        return;             // 立即停止所有更新
    }

    // --- 2. 障碍物生成（复用原 Python 逻辑）---
    // 生成逻辑应该放在状态更新循环之外，但我们先按照您的 C 语言草稿将其放在后面。

    // --- 3. 核心状态转换逻辑 (复刻 Python 脚本) ---

    int i = 0;
    while (i < COL)
    {
        if (grass_status[i] == 0)
        {
            i = i + 1;
        }
        else if (grass_status[i] == 1)
        {
            if (i == COL - 1)
            {
                // 最右侧，状态 1 -> 2
                grass_status[i] = 2;
                return; // 提前退出循环 (Python 逻辑)
            }
            else
            {
                // 状态 1 -> 2，并在 i+1 传递状态 4 (分裂/移动)
                grass_status[i] = 2;
                grass_status[i + 1] = 4;
                i = i + 2; // 跳过下一格 (i+1)
            }
        }
        else if (grass_status[i] == 2)
        {
            // 状态 2 -> 3，并在 i-1 传递状态 1，i+1 设为 0 (移动/动画)
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
        else if (grass_status[i] == 3)
        {
            // 状态 3 -> 4 (帧动画)
            grass_status[i] = 4;
            i = i + 1;
        }
        else // grass_status[i] == 4
        {
            // 状态 4 -> 0 (消失)
            grass_status[i] = 0;
            if (i == 0)
            {
                score = score + 1;  // 障碍物成功通过，得分 + 1
            }
            i = i + 1;
        }
    }

    // --- 4. 新障碍物生成（生成逻辑应该放在这里，忠实复刻 Python 原理）---

    srand(HAL_GetTick());
    // 只有当最右侧障碍物远离恐龙（距离 > 8）时，才有几率生成新的障碍物
    if (get_grass_age() > 8 && (rand() % 10) > 8)
    {
        // 检查最右侧是否为 0 是不必要的，因为前面的逻辑已经确保了这一点。
        // 但我们检查一下 COL-1 是否为 0 以确保安全（如果前面的逻辑没跑完）。
        if (grass_status[COL - 1] == 0)
        {
            grass_status[COL - 1] = 1; // 新草地从最右侧开始，状态 1
        }
    }
}
/**
 * @brief  处理恐龙的跳跃状态和动画。
 * 跳跃过程使用 16 个状态 (0-15) 来表示一个完整的跳跃周期。
 */
void Game_Update_Jump(void)
{
    // 确保 jump_status, jump_order 是全局 volatile 变量
    extern volatile bool jump_order;
    extern volatile int8_t jump_status; // 假设 jump_status 在 0-15 循环

    // 逻辑：如果处于地面状态(0)且有跳跃命令(jump_order)，则状态变为 1。否则，循环计数。

    if (jump_status == 0)
    {
        if (jump_order)
        {
            jump_status = 1;  // 处理跳跃命令：从 0 -> 1 开始跳
        }
    }
    else
    {
        // 循环 1 -> 2 -> ... -> JUMP_CYCLE-1 -> 0
        jump_status = (jump_status + 1) % JUMP_CYCLE;
    }

    // jump order 应该在处理后清除
    // 如果在跳跃过程中收到 jump_order，它会被忽略（因为 jump_status != 0），然后在这里被清除。
    jump_order = false;
}