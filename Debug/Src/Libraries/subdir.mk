################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Libraries/W25Qxx.c 

OBJS += \
./Src/Libraries/W25Qxx.o 

C_DEPS += \
./Src/Libraries/W25Qxx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Libraries/%.o Src/Libraries/%.su Src/Libraries/%.cyclo: ../Src/Libraries/%.c Src/Libraries/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F401xE -c -I../Inc -I"../$(ProjDirPath)/Headers/CMSIS/Include" -I"../$(ProjDirPath)/Headers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-Libraries

clean-Src-2f-Libraries:
	-$(RM) ./Src/Libraries/W25Qxx.cyclo ./Src/Libraries/W25Qxx.d ./Src/Libraries/W25Qxx.o ./Src/Libraries/W25Qxx.su

.PHONY: clean-Src-2f-Libraries

