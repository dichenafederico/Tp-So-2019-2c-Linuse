################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/biblioSuse.c \
../src/libSuseUtils.c 

OBJS += \
./src/biblioSuse.o \
./src/libSuseUtils.o 

C_DEPS += \
./src/biblioSuse.d \
./src/libSuseUtils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC" -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


