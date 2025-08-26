set(CPU_PARAMETERS ${CPU_PARAMETERS}
    -mthumb
    -mcpu=cortex-m0plus
    -mfloat-abi=soft
)

set(compiler_define ${compiler_define}
    "USE_HAL_DRIVER"
    "STM32L053xx"
)
