################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/search/map_cmd.c \
../Core/Src/search/map_flash.c \
../Core/Src/search/search.c 

C_DEPS += \
./Core/Src/search/map_cmd.d \
./Core/Src/search/map_flash.d \
./Core/Src/search/search.d 

OBJS += \
./Core/Src/search/map_cmd.o \
./Core/Src/search/map_flash.o \
./Core/Src/search/search.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/search/%.o Core/Src/search/%.su Core/Src/search/%.cyclo: ../Core/Src/search/%.c Core/Src/search/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32G474xx -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=16000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=0 -DINSTRUCTION_CACHE_ENABLE=1 -DDATA_CACHE_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-search

clean-Core-2f-Src-2f-search:
	-$(RM) ./Core/Src/search/map_cmd.cyclo ./Core/Src/search/map_cmd.d ./Core/Src/search/map_cmd.o ./Core/Src/search/map_cmd.su ./Core/Src/search/map_flash.cyclo ./Core/Src/search/map_flash.d ./Core/Src/search/map_flash.o ./Core/Src/search/map_flash.su ./Core/Src/search/search.cyclo ./Core/Src/search/search.d ./Core/Src/search/search.o ./Core/Src/search/search.su

.PHONY: clean-Core-2f-Src-2f-search

