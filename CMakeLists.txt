cmake_minimum_required(VERSION 3.25)

# ==============================================================================
# 1. 系统底层设置
# ==============================================================================
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ==============================================================================
# 2. 项目名称
# ==============================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

project(mes204 C ASM)

# ==============================================================================
# 3. 编译器参数
# ==============================================================================
set(MCU_FLAGS
        -mcpu=cortex-m4
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
)

add_compile_definitions(
        USE_HAL_DRIVER
        STM32F446xx
)

add_compile_options(
        ${MCU_FLAGS}
        -Wall
        -fdata-sections
        -ffunction-sections
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_options(-g -gdwarf-2)
endif()

# ==============================================================================
# 4. 源文件清单
# ==============================================================================
set(SOURCES
        startup_stm32f446xx.s
        Core/Src/main.c
        Core/Src/lcd.c
        Core/Src/stm32f4xx_it.c
        Core/Src/stm32f4xx_hal_msp.c
        Core/Src/system_stm32f4xx.c
        Core/Src/sysmem.c
        Core/Src/syscalls.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
        Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
)

# ==============================================================================
# 5. 头文件路径
# ==============================================================================
set(INCLUDE_DIRS
        Core/Inc
        Drivers/STM32F4xx_HAL_Driver/Inc
        Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
        Drivers/CMSIS/Device/ST/STM32F4xx/Include
        Drivers/CMSIS/Include
)

# ==============================================================================
# 6. 生成可执行文件
# ==============================================================================
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

# 【关键修复】强制后缀名为 .elf
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".elf")

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

# ==============================================================================
# 7. 链接设置
# ==============================================================================
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/STM32F446XX_FLASH.ld")

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        ${MCU_FLAGS}
        -T${LINKER_SCRIPT}
        -specs=nano.specs
        -lc
        -lm
        -Wl,-Map=${CMAKE_PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
)

# ==============================================================================
# 8. 自动生成 Hex 和 Bin
# ==============================================================================
get_filename_component(TOOLCHAIN_BIN_DIR ${CMAKE_C_COMPILER} DIRECTORY)
set(OBJCOPY "${TOOLCHAIN_BIN_DIR}/arm-none-eabi-objcopy.exe")
set(SIZE "${TOOLCHAIN_BIN_DIR}/arm-none-eabi-size.exe")

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${OBJCOPY} -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
        COMMAND ${OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
        COMMAND ${SIZE} ${CMAKE_PROJECT_NAME}.elf
        COMMENT "Generating .hex and .bin files..."
)