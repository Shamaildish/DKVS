################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ConsistingHashing.cpp \
../src/DKVSClient.cpp \
../src/DKVSIndexer.cpp \
../src/DKVSServer.cpp \
../src/GeneralFunctions.cpp \
../src/HashTable.cpp \
../src/TCPConnection.cpp \
../src/main.cpp 

OBJS += \
./src/ConsistingHashing.o \
./src/DKVSClient.o \
./src/DKVSIndexer.o \
./src/DKVSServer.o \
./src/GeneralFunctions.o \
./src/HashTable.o \
./src/TCPConnection.o \
./src/main.o 

CPP_DEPS += \
./src/ConsistingHashing.d \
./src/DKVSClient.d \
./src/DKVSIndexer.d \
./src/DKVSServer.d \
./src/GeneralFunctions.d \
./src/HashTable.d \
./src/TCPConnection.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


