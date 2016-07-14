################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include test/tlb/subdir.mk
-include test/pedidos/nucleo/subdir.mk
-include test/marcos/subdir.mk
-include test/consola/subdir.mk
-include test/commonsV2/subdir.mk
-include test/algoritmos/clock-m/subdir.mk
-include test/algoritmos/clock/subdir.mk
-include test/algoritmos/subdir.mk
-include src/tlb/subdir.mk
-include src/tabla_de_paginas/subdir.mk
-include src/pedidos/interfaces/subdir.mk
-include src/pedidos/subdir.mk
-include src/hilos/subdir.mk
-include src/consola/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: pre-build main-build

# Main-build Target
main-build: UMC

# Tool invocations
UMC: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "UMC" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) UMC
	-@echo ' '

pre-build:
	-make clean
	-@echo ' '

post-build:
	-cp -f UMC /home/utnso/tp-2016-1c-El-algoritmo-del-ritmo/UMC
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
