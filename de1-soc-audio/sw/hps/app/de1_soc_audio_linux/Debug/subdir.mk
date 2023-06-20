################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../alt_generalpurpose_io.c \
../audio.c \
../hps_linux.c 

C_DEPS += \
./alt_generalpurpose_io.d \
./audio.d \
./hps_linux.d 

OBJS += \
./alt_generalpurpose_io.o \
./audio.o \
./hps_linux.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 7.5.0 [arm-linux-gnueabihf]'
	arm-linux-gnueabihf-gcc.exe -Dsoc_cv_av -I"C:\intelFPGA\20.1\embedded\ip\altera\hps\altera_hps\hwlib\include" -I"C:\intelFPGA\20.1\embedded\ip\altera\hps\altera_hps\hwlib\include\soc_cv_av" -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean--2e-

clean--2e-:
	-$(RM) ./alt_generalpurpose_io.d ./alt_generalpurpose_io.o ./audio.d ./audio.o ./hps_linux.d ./hps_linux.o

.PHONY: clean--2e-

