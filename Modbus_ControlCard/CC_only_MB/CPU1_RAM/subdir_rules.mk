################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1391903649: ../c2000.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1271/ccs/utils/sysconfig_1.20.0/sysconfig_cli.bat" --script "C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/c2000.syscfg" -o "syscfg" -s "C:/ti/c2000/C2000Ware_5_01_00_00/.metadata/sdk.json" -d "F28004x" --package F28004x_100PZ --part F28004x_100PZ --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-1391903649 ../c2000.syscfg
syscfg/board.h: build-1391903649
syscfg/board.cmd.genlibs: build-1391903649
syscfg/board.opt: build-1391903649
syscfg/pinmux.csv: build-1391903649
syscfg/c2000ware_libraries.cmd.genlibs: build-1391903649
syscfg/c2000ware_libraries.opt: build-1391903649
syscfg/c2000ware_libraries.c: build-1391903649
syscfg/c2000ware_libraries.h: build-1391903649
syscfg/clocktree.h: build-1391903649
syscfg: build-1391903649

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB" --include_path="C:/ti/c2000/C2000Ware_5_01_00_00" --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/mb_slave" --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/mb_master" --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/device" --include_path="C:/ti/c2000/C2000Ware_5_01_00_00/driverlib/f28004x/driverlib" --include_path="C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/CPU1_RAM/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB" --include_path="C:/ti/c2000/C2000Ware_5_01_00_00" --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/mb_slave" --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/mb_master" --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/device" --include_path="C:/ti/c2000/C2000Ware_5_01_00_00/driverlib/f28004x/driverlib" --include_path="C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


