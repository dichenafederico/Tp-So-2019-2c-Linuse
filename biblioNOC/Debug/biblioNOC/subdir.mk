################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../biblioNOC/conexiones.c \
../biblioNOC/paquetes.c \
../biblioNOC/serializaciones.c 

OBJS += \
./biblioNOC/conexiones.o \
./biblioNOC/paquetes.o \
./biblioNOC/serializaciones.o 

C_DEPS += \
./biblioNOC/conexiones.d \
./biblioNOC/paquetes.d \
./biblioNOC/serializaciones.d 


# Each subdirectory must supply rules for building sources it contributes
biblioNOC/%.o: ../biblioNOC/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


