set(TARGET_INCLUDES
  ${TARGET_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L0xx/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy
  CACHE INTERNAL ""
)

set(TARGET_SOURCES
  ${TARGET_SOURCES}
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/syscalls.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/system_stm32l0xx.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/stm32l0xx_hal_msp.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/stm32l0xx_it.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/sysmem.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_exti.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ramfunc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c.c
  CACHE INTERNAL ""
)

set(linker_script ${CMAKE_CURRENT_SOURCE_DIR}/STM32L053XX_FLASH.ld CACHE INTERNAL "")

message("Target include paths: " ${TARGET_INCLUDES})
message("Target sources: " ${TARGET_SOURCES})
message("Linker script: " ${linker_script})
