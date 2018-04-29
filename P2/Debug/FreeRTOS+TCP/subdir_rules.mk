################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS+TCP/FreeRTOS_ARP.obj: ../FreeRTOS+TCP/FreeRTOS_ARP.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS+TCP/FreeRTOS_ARP.d_raw" --obj_directory="FreeRTOS+TCP" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS+TCP/FreeRTOS_DHCP.obj: ../FreeRTOS+TCP/FreeRTOS_DHCP.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS+TCP/FreeRTOS_DHCP.d_raw" --obj_directory="FreeRTOS+TCP" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS+TCP/FreeRTOS_DNS.obj: ../FreeRTOS+TCP/FreeRTOS_DNS.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS+TCP/FreeRTOS_DNS.d_raw" --obj_directory="FreeRTOS+TCP" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS+TCP/FreeRTOS_IP.obj: ../FreeRTOS+TCP/FreeRTOS_IP.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS+TCP/FreeRTOS_IP.d_raw" --obj_directory="FreeRTOS+TCP" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS+TCP/FreeRTOS_Sockets.obj: ../FreeRTOS+TCP/FreeRTOS_Sockets.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS+TCP/FreeRTOS_Sockets.d_raw" --obj_directory="FreeRTOS+TCP" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS+TCP/FreeRTOS_Stream_Buffer.obj: ../FreeRTOS+TCP/FreeRTOS_Stream_Buffer.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS+TCP/FreeRTOS_Stream_Buffer.d_raw" --obj_directory="FreeRTOS+TCP" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS+TCP/FreeRTOS_TCP_IP.obj: ../FreeRTOS+TCP/FreeRTOS_TCP_IP.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS+TCP/FreeRTOS_TCP_IP.d_raw" --obj_directory="FreeRTOS+TCP" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS+TCP/FreeRTOS_TCP_WIN.obj: ../FreeRTOS+TCP/FreeRTOS_TCP_WIN.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS+TCP/FreeRTOS_TCP_WIN.d_raw" --obj_directory="FreeRTOS+TCP" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS+TCP/FreeRTOS_UDP_IP.obj: ../FreeRTOS+TCP/FreeRTOS_UDP_IP.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS+TCP/FreeRTOS_UDP_IP.d_raw" --obj_directory="FreeRTOS+TCP" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


