################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/usr/share/arduino/libraries/Wire/utility/twi.c 

OBJS += \
./Wire/utility/twi.o 

C_DEPS += \
./Wire/utility/twi.d 


# Each subdirectory must supply rules for building sources it contributes
Wire/utility/twi.o: /usr/share/arduino/libraries/Wire/utility/twi.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/usr/share/arduino/hardware/arduino/cores/arduino" -I"/usr/share/arduino/hardware/arduino/variants/standard" -I"/home/semion/workspace/AClock" -I"/usr/share/arduino/libraries/Wire" -I"/usr/share/arduino/libraries/Wire/utility" -D__IN_ECLIPSE__=1 -DARDUINO=100 -DUSB_PID= -DUSB_VID= -Wall -Os -g -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


