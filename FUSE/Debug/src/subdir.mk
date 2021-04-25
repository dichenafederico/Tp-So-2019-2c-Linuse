################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/fuse_serializaciones.c \
../src/sac_cli.c 

OBJS += \
./src/fuse_serializaciones.o \
./src/sac_cli.o 

C_DEPS += \
./src/fuse_serializaciones.d \
./src/sac_cli.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC" -I"/home/utnso/workspace/tp-2019-2c-No-C-Nada/biblioNOC/Debug" -O0 -g3 -Wall -c -fmessage-length=0 -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


