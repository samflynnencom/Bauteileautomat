################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../tm4c123gh6pm.cmd 

CFG_SRCS += \
../main.cfg 

C_SRCS += \
../EK_TM4C123GXL.c \
../main.c 

OBJS += \
./EK_TM4C123GXL.obj \
./main.obj 

C_DEPS += \
./EK_TM4C123GXL.pp \
./main.pp 

GEN_MISC_DIRS += \
./configPkg/ 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_OPTS += \
./configPkg/compiler.opt 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

C_DEPS__QUOTED += \
"EK_TM4C123GXL.pp" \
"main.pp" 

OBJS__QUOTED += \
"EK_TM4C123GXL.obj" \
"main.obj" 

C_SRCS__QUOTED += \
"../EK_TM4C123GXL.c" \
"../main.c" 

GEN_CMDS__FLAG += \
-l"./configPkg/linker.cmd" 

GEN_OPTS__FLAG += \
--cmd_file="./configPkg/compiler.opt" 


