################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/drv_microphone/src/microphone.c 

OBJS += \
./Drivers/drv_microphone/src/microphone.o 

C_DEPS += \
./Drivers/drv_microphone/src/microphone.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/drv_microphone/src/%.o Drivers/drv_microphone/src/%.su Drivers/drv_microphone/src/%.cyclo: ../Drivers/drv_microphone/src/%.c Drivers/drv_microphone/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../USB_HOST/App -I../USB_HOST/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"../Drivers/drv_at_command/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-drv_microphone-2f-src

clean-Drivers-2f-drv_microphone-2f-src:
	-$(RM) ./Drivers/drv_microphone/src/microphone.cyclo ./Drivers/drv_microphone/src/microphone.d ./Drivers/drv_microphone/src/microphone.o ./Drivers/drv_microphone/src/microphone.su

.PHONY: clean-Drivers-2f-drv_microphone-2f-src

