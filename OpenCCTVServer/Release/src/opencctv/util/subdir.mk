################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/opencctv/util/Config.cpp \
../src/opencctv/util/SharedLibLoader.cpp \
../src/opencctv/util/Unzipper.cpp \
../src/opencctv/util/Util.cpp 

OBJS += \
./src/opencctv/util/Config.o \
./src/opencctv/util/SharedLibLoader.o \
./src/opencctv/util/Unzipper.o \
./src/opencctv/util/Util.o 

CPP_DEPS += \
./src/opencctv/util/Config.d \
./src/opencctv/util/SharedLibLoader.d \
./src/opencctv/util/Unzipper.d \
./src/opencctv/util/Util.d 


# Each subdirectory must supply rules for building sources it contributes
src/opencctv/util/%.o: ../src/opencctv/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


