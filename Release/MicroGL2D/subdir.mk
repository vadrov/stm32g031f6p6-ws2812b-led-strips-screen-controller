################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MicroGL2D/microgl2d.c 

OBJS += \
./MicroGL2D/microgl2d.o 

C_DEPS += \
./MicroGL2D/microgl2d.d 


# Each subdirectory must supply rules for building sources it contributes
MicroGL2D/%.o MicroGL2D/%.su MicroGL2D/%.cyclo: ../MicroGL2D/%.c MicroGL2D/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -DSTM32G031xx -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f8p6_led_lent_controller/Display" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f8p6_led_lent_controller/MicroGL2D" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f8p6_led_lent_controller/FATFS" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f8p6_led_lent_controller/Encoder" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f8p6_led_lent_controller/Keyboard" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f8p6_led_lent_controller/MyString" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f8p6_led_lent_controller/FileManager" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f8p6_led_lent_controller/WS2812B" -I"C:/Users/Vadim/STM32Cube/Repository/stm32g031f8p6_led_lent_controller/AVI" -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-MicroGL2D

clean-MicroGL2D:
	-$(RM) ./MicroGL2D/microgl2d.cyclo ./MicroGL2D/microgl2d.d ./MicroGL2D/microgl2d.o ./MicroGL2D/microgl2d.su

.PHONY: clean-MicroGL2D

