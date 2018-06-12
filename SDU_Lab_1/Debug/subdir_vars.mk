################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
C:/ti/controlSUITE/device_support/f2833x/v141/DSP2833x_common/cmd/28335_RAM_lnk.cmd \
C:/ti/controlSUITE/device_support/f2833x/v141/DSP2833x_headers/cmd/DSP2833x_Headers_nonBIOS.cmd 

ASM_SRCS += \
../DSP2833x_ADC_cal.asm \
C:/ti/controlSUITE/device_support/f2833x/v141/DSP2833x_common/source/DSP2833x_CodeStartBranch.asm \
../DSP2833x_usDelay.asm 

C_SRCS += \
C:/ti/controlSUITE/device_support/f2833x/v141/DSP2833x_headers/source/DSP2833x_GlobalVariableDefs.c \
../DSP2833x_SysCtrl.c \
../pwm.c 

OBJS += \
./DSP2833x_ADC_cal.obj \
./DSP2833x_CodeStartBranch.obj \
./DSP2833x_GlobalVariableDefs.obj \
./DSP2833x_SysCtrl.obj \
./DSP2833x_usDelay.obj \
./pwm.obj 

ASM_DEPS += \
./DSP2833x_ADC_cal.d \
./DSP2833x_CodeStartBranch.d \
./DSP2833x_usDelay.d 

C_DEPS += \
./DSP2833x_GlobalVariableDefs.d \
./DSP2833x_SysCtrl.d \
./pwm.d 

C_DEPS__QUOTED += \
"DSP2833x_GlobalVariableDefs.d" \
"DSP2833x_SysCtrl.d" \
"pwm.d" 

OBJS__QUOTED += \
"DSP2833x_ADC_cal.obj" \
"DSP2833x_CodeStartBranch.obj" \
"DSP2833x_GlobalVariableDefs.obj" \
"DSP2833x_SysCtrl.obj" \
"DSP2833x_usDelay.obj" \
"pwm.obj" 

ASM_DEPS__QUOTED += \
"DSP2833x_ADC_cal.d" \
"DSP2833x_CodeStartBranch.d" \
"DSP2833x_usDelay.d" 

ASM_SRCS__QUOTED += \
"../DSP2833x_ADC_cal.asm" \
"C:/ti/controlSUITE/device_support/f2833x/v141/DSP2833x_common/source/DSP2833x_CodeStartBranch.asm" \
"../DSP2833x_usDelay.asm" 

C_SRCS__QUOTED += \
"C:/ti/controlSUITE/device_support/f2833x/v141/DSP2833x_headers/source/DSP2833x_GlobalVariableDefs.c" \
"../DSP2833x_SysCtrl.c" \
"../pwm.c" 


