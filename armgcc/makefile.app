######################################
# target
######################################
TARGET = FreeJoy


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O3


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build/app

######################################
# source
######################################
# C sources
C_SOURCES =  \
../application/Src/analog.c \
../application/Src/axis_to_buttons.c \
../application/Src/buttons.c \
../utils/crc16.c \
../application/Src/encoders.c \
../application/Src/config.c \
../application/Src/leds.c \
../application/Src/main.c \
../application/Src/periphery.c \
../application/Src/tle5011.c \
../application/Src/tle5012.c \
../application/Src/as5600.c \
../application/Src/as5048a.c \
../application/Src/ads1115.c \
../application/Src/mlx90363.c \
../application/Src/mlx90393.c \
../application/Src/mcp320x.c \
../application/Src/shift_registers.c \
../application/Src/spi.c \
../application/Src/i2c.c \
../application/Src/stm32f10x_it.c \
../application/Src/usb_desc.c \
../application/Src/usb_endp.c \
../application/Src/usb_hw.c \
../application/Src/usb_istr.c \
../application/Src/usb_prop.c \
../application/Src/usb_pwr.c \
../Drivers/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c \
../Drivers/STM32F10x_StdPeriph_Driver/src/misc.c \
../Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c \
../Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c \
../Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c \
../Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
../Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
../Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c \
../Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c \
../Drivers/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c \
../Drivers/STM32_USB-FS-Device_Driver/src/usb_core.c \
../Drivers/STM32_USB-FS-Device_Driver/src/usb_init.c \
../Drivers/STM32_USB-FS-Device_Driver/src/usb_int.c \
../Drivers/STM32_USB-FS-Device_Driver/src/usb_mem.c \
../Drivers/STM32_USB-FS-Device_Driver/src/usb_regs.c \
../Drivers/STM32_USB-FS-Device_Driver/src/usb_sil.c \
#../SEGGER/SEGGER/SEGGER_SYSVIEW.c \
#../SEGGER/SEGGER/SEGGER_RTT.c \
#../SEGGER/SEGGER/SEGGER_RTT_printf.c \
#../SEGGER/Sample/NoOS/Config/Cortex-M/SEGGER_SYSVIEW_Config_NoOS.c 

# ASM sources
ASM_SOURCES =  \
startup_stm32f103xb.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_STDPERIPH_DRIVER \
-DSTM32F10X_MD


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-I../Drivers/CMSIS/CM3/CoreSupport \
-I../Drivers/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
-I../Drivers/STM32F10x_StdPeriph_Driver/inc \
-I../Drivers/STM32_USB-FS-Device_Driver/inc \
-I../application/Inc \
-I../utils \
#-I../SEGGER/SEGGER \
#-I../SEGGER/Config 

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = linker_app.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex 
#all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir -p $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***