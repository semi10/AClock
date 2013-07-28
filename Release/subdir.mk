################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AClock.cpp \
../MSGEQ7.cpp \
../RHT03.cpp \
../RTClib.cpp \
../ht1632c.cpp 

OBJS += \
./AClock.o \
./MSGEQ7.o \
./RHT03.o \
./RTClib.o \
./ht1632c.o 

CPP_DEPS += \
./AClock.d \
./MSGEQ7.d \
./RHT03.d \
./RTClib.d \
./ht1632c.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/usr/share/arduino/hardware/arduino/cores/arduino" -I"/usr/share/arduino/hardware/arduino/variants/standard" -I"/home/semion/workspace/AClock" -I"/usr/share/arduino/libraries/Wire" -I"/usr/share/arduino/libraries/Wire/utility" -D__IN_ECLIPSE__=1 -DUSB_VID= -DUSB_PID= -DARDUINO=100 -Wall -Os -ffunction-sections -fdata-sections -fno-exceptions -g -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -c -o "$@" -x c++ "$<"
	@echo 'Finished building: $<'
	@echo ' '


