################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include WS2812B/subdir.mk
-include MyString/subdir.mk
-include Keyboard/subdir.mk
-include FileManager/subdir.mk
-include FATFS/subdir.mk
-include Encoder/subdir.mk
-include Drivers/STM32G0xx_HAL_Driver/Src/subdir.mk
-include Display/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include AVI/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := stm32g031f6p6-led-lent-screen-controller
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
stm32g031f6p6-led-lent-screen-controller.elf \

MAP_FILES += \
stm32g031f6p6-led-lent-screen-controller.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
stm32g031f6p6-led-lent-screen-controller.list \

OBJCOPY_HEX += \
stm32g031f6p6-led-lent-screen-controller.hex \

OBJCOPY_BIN += \
stm32g031f6p6-led-lent-screen-controller.bin \


# All Target
all: main-build

# Main-build Target
main-build: stm32g031f6p6-led-lent-screen-controller.elf secondary-outputs

# Tool invocations
stm32g031f6p6-led-lent-screen-controller.elf stm32g031f6p6-led-lent-screen-controller.map: $(OBJS) $(USER_OBJS) C:\Users\Vadim\STM32Cube\Repository\stm32g031f6p6-led-lent-screen-controller\STM32G031F6PX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "stm32g031f6p6-led-lent-screen-controller.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0plus -T"C:\Users\Vadim\STM32Cube\Repository\stm32g031f6p6-led-lent-screen-controller\STM32G031F6PX_FLASH.ld" --specs=nosys.specs -Wl,-Map="stm32g031f6p6-led-lent-screen-controller.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

stm32g031f6p6-led-lent-screen-controller.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "stm32g031f6p6-led-lent-screen-controller.list"
	@echo 'Finished building: $@'
	@echo ' '

stm32g031f6p6-led-lent-screen-controller.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "stm32g031f6p6-led-lent-screen-controller.hex"
	@echo 'Finished building: $@'
	@echo ' '

stm32g031f6p6-led-lent-screen-controller.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "stm32g031f6p6-led-lent-screen-controller.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout stm32g031f6p6-led-lent-screen-controller.bin stm32g031f6p6-led-lent-screen-controller.elf stm32g031f6p6-led-lent-screen-controller.hex stm32g031f6p6-led-lent-screen-controller.list stm32g031f6p6-led-lent-screen-controller.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
