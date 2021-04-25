################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SAC-Server.c \
../src/sac_fs_handlers.c \
../src/sac_server_serializaciones.c 

OBJS += \
./src/SAC-Server.o \
./src/sac_fs_handlers.o \
./src/sac_server_serializaciones.o 

C_DEPS += \
./src/SAC-Server.d \
./src/sac_fs_handlers.d \
./src/sac_server_serializaciones.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC -O0 -g3 -Wall -c -fmessage-length=0 -D_FILE_OFFSET_BITS=64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


