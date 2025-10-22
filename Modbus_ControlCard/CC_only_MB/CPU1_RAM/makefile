################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS

GEN_OPTS__FLAG := --cmd_file="syscfg/board.opt" --cmd_file="syscfg/c2000ware_libraries.opt" 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./syscfg/board.obj" \
"./syscfg/c2000ware_libraries.obj" \
"./main.obj" \
"./test_spi_master.obj" \
"./timetask.obj" \
"./device/device.obj" \
"./device/f28004x_codestartbranch.obj" \
"./mb_master/CRC16m.obj" \
"./mb_master/ModbusCommFunc.obj" \
"./mb_master/ModbusMaster.obj" \
"./mb_master/ModbusSci.obj" \
"./mb_master/Modbus_Ctrl.obj" \
"./mb_slave/CRC16.obj" \
"./mb_slave/ModbusFunction.obj" \
"./mb_slave/ModbusPort.obj" \
"./mb_slave/ModbusSlave.obj" \
"./mb_slave/linkVariables.obj" \
"./mb_slave/tbmenu.obj" \
"../28004x_generic_ram_lnk.cmd" \
"C:/ti/c2000/C2000Ware_5_01_00_00/driverlib/f28004x/driverlib/ccs/Debug/driverlib.lib" \
$(GEN_CMDS__FLAG) \
-lc2000ware_libraries.cmd.genlibs \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include device/subdir_vars.mk
-include mb_master/subdir_vars.mk
-include mb_slave/subdir_vars.mk
-include subdir_rules.mk
-include device/subdir_rules.mk
-include mb_master/subdir_rules.mk
-include mb_slave/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
CC_only_MB.out 

EXE_OUTPUTS__QUOTED += \
"CC_only_MB.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "CC_only_MB.out"

# Tool invocations
CC_only_MB.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --define=DEBUG --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi -z -m"CC_only_MB.map" --heap_size=0x200 --stack_size=0x3F8 --warn_sections -i"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/lib" -i"C:/ti/c2000/C2000Ware_5_01_00_00" -i"C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/CPU1_RAM/syscfg" -i"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="CC_only_MB_linkInfo.xml" --entry_point=code_start --rom_model -o "CC_only_MB.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "syscfg\board.obj" "syscfg\c2000ware_libraries.obj" "main.obj" "test_spi_master.obj" "timetask.obj" "device\device.obj" "device\f28004x_codestartbranch.obj" "mb_master\CRC16m.obj" "mb_master\ModbusCommFunc.obj" "mb_master\ModbusMaster.obj" "mb_master\ModbusSci.obj" "mb_master\Modbus_Ctrl.obj" "mb_slave\CRC16.obj" "mb_slave\ModbusFunction.obj" "mb_slave\ModbusPort.obj" "mb_slave\ModbusSlave.obj" "mb_slave\linkVariables.obj" "mb_slave\tbmenu.obj" 
	-$(RM) "syscfg\board.d" "syscfg\c2000ware_libraries.d" "main.d" "test_spi_master.d" "timetask.d" "device\device.d" "mb_master\CRC16m.d" "mb_master\ModbusCommFunc.d" "mb_master\ModbusMaster.d" "mb_master\ModbusSci.d" "mb_master\Modbus_Ctrl.d" "mb_slave\CRC16.d" "mb_slave\ModbusFunction.d" "mb_slave\ModbusPort.d" "mb_slave\ModbusSlave.d" "mb_slave\linkVariables.d" "mb_slave\tbmenu.d" 
	-$(RM) "device\f28004x_codestartbranch.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-echo "========= Build of the CLB simulation has moved to the CLB Tool. Click the '?' icon by the _Generate CLB Simulation File_ enable for more details (located in the Global Parameters of the Tile Design SysConfig module) ============="
	-if 0 == 1  C:/ti/ccs1271/ccs/tools/node/node.exe "C:/ti/c2000/C2000Ware_5_01_00_00/utilities/clb_tool/clb_syscfg/dot_file_libraries/clbDotUtility.js"  "C:/ti/c2000/C2000Ware_5_01_00_00/utilities/clb_tool/clb_syscfg/" "C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/CPU1_RAM/syscfg" "C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/CPU1_RAM/syscfg/clb.dot"
	-if 0 == 1  mkdir "C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/CPU1_RAM/diagrams"
	-if 0 == 1  C:/ti/ccs1271/ccs/tools/node/node.exe "C:/ti/c2000/C2000Ware_5_01_00_00/driverlib/.meta/generate_diagrams.js" "C:/ti/c2000/C2000Ware_5_01_00_00" "C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/CPU1_RAM/diagrams" "C:/Users/roger_lin/Documents/GitHub/test-environment/Modbus_ControlCard/CC_only_MB/CPU1_RAM/syscfg"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

