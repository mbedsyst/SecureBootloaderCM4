################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Drivers/CRC.c \
../Src/Drivers/FLASH.c \
../Src/Drivers/LED.c \
../Src/Drivers/SPI.c \
../Src/Drivers/SYSTICK.c \
../Src/Drivers/TIM.c \
../Src/Drivers/UART.c 

OBJS += \
./Src/Drivers/CRC.o \
./Src/Drivers/FLASH.o \
./Src/Drivers/LED.o \
./Src/Drivers/SPI.o \
./Src/Drivers/SYSTICK.o \
./Src/Drivers/TIM.o \
./Src/Drivers/UART.o 

C_DEPS += \
./Src/Drivers/CRC.d \
./Src/Drivers/FLASH.d \
./Src/Drivers/LED.d \
./Src/Drivers/SPI.d \
./Src/Drivers/SYSTICK.d \
./Src/Drivers/TIM.d \
./Src/Drivers/UART.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Drivers/%.o Src/Drivers/%.su Src/Drivers/%.cyclo: ../Src/Drivers/%.c Src/Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F401xE -c -I../Inc -I"../$(ProjDirPath)/Headers/CMSIS/Include" -I"../$(ProjDirPath)/Headers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-Drivers

clean-Src-2f-Drivers:
	-$(RM) ./Src/Drivers/CRC.cyclo ./Src/Drivers/CRC.d ./Src/Drivers/CRC.o ./Src/Drivers/CRC.su ./Src/Drivers/FLASH.cyclo ./Src/Drivers/FLASH.d ./Src/Drivers/FLASH.o ./Src/Drivers/FLASH.su ./Src/Drivers/LED.cyclo ./Src/Drivers/LED.d ./Src/Drivers/LED.o ./Src/Drivers/LED.su ./Src/Drivers/SPI.cyclo ./Src/Drivers/SPI.d ./Src/Drivers/SPI.o ./Src/Drivers/SPI.su ./Src/Drivers/SYSTICK.cyclo ./Src/Drivers/SYSTICK.d ./Src/Drivers/SYSTICK.o ./Src/Drivers/SYSTICK.su ./Src/Drivers/TIM.cyclo ./Src/Drivers/TIM.d ./Src/Drivers/TIM.o ./Src/Drivers/TIM.su ./Src/Drivers/UART.cyclo ./Src/Drivers/UART.d ./Src/Drivers/UART.o ./Src/Drivers/UART.su

.PHONY: clean-Src-2f-Drivers

