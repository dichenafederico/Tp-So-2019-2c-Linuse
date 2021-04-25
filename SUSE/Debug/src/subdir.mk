################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SUSE.c \
../src/suseDefs.c 

OBJS += \
./src/SUSE.o \
./src/suseDefs.o 

C_DEPS += \
./src/SUSE.d \
./src/suseDefs.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC/biblioNOC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


