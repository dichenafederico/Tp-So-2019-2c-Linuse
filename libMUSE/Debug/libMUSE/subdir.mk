################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libMUSE/libMUSE.c \
../libMUSE/util.c 

OBJS += \
./libMUSE/libMUSE.o \
./libMUSE/util.o 

C_DEPS += \
./libMUSE/libMUSE.d \
./libMUSE/util.d 


# Each subdirectory must supply rules for building sources it contributes
libMUSE/%.o: ../libMUSE/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


