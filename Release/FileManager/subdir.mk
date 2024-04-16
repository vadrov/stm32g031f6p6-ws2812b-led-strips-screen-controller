################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FileManager/filemanager.c \
../FileManager/icons.c 

OBJS += \
./FileManager/filemanager.o \
./FileManager/icons.o 

C_DEPS += \
./FileManager/filemanager.d \
./FileManager/icons.d 


# Each subdirectory must supply rules for building sources it contributes
FileManager/%.o FileManager/%.su FileManager/%.cyclo: ../FileManager/%.c FileManager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -DSTM32G031xx -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6-led-lent-screen-controller/Display" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6-led-lent-screen-controller/FATFS" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6-led-lent-screen-controller/Encoder" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6-led-lent-screen-controller/Keyboard" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6-led-lent-screen-controller/MyString" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6-led-lent-screen-controller/FileManager" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6-led-lent-screen-controller/WS2812B" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f6p6-led-lent-screen-controller/AVI" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-FileManager

clean-FileManager:
	-$(RM) ./FileManager/filemanager.cyclo ./FileManager/filemanager.d ./FileManager/filemanager.o ./FileManager/filemanager.su ./FileManager/icons.cyclo ./FileManager/icons.d ./FileManager/icons.o ./FileManager/icons.su

.PHONY: clean-FileManager

