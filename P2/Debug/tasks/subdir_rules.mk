################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
tasks/comm_task.obj: ../tasks/comm_task.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="tasks/comm_task.d_raw" --obj_directory="tasks" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

tasks/pressure_task.obj: ../tasks/pressure_task.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="tasks/pressure_task.d_raw" --obj_directory="tasks" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

tasks/uv_task.obj: ../tasks/uv_task.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/VIJOY-PC/workspace_v8/apes_p2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178/driverlib" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source" --include_path="C:/Users/VIJOY-PC/Downloads/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="tasks/uv_task.d_raw" --obj_directory="tasks" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


