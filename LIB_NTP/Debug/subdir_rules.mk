################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
NTPClient.obj: ../NTPClient.cpp $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --abi=eabi -me --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/ti/CC3200SDK/cc3200-sdk/simplelink" --include_path="C:/ti/CC3200SDK/cc3200-sdk/oslib" --include_path="C:/ti/CC3200SDK/cc3200-sdk/simplelink/include" --include_path="C:/ti/CC3200SDK/cc3200-sdk/simplelink/source" --include_path="C:/ti/CC3200SDK/cc3200-sdk/driverlib/ccs" --include_path="C:/ti/CC3200SDK/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK/cc3200-sdk/example/common" --include_path="C:/ti/CC3200SDK/cc3200-sdk/inc" -g --define=cc3200 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="NTPClient.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


