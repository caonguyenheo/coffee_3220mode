SIMPLELINK_CC32XX_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/imports.mak


SYSCONFIG_PRODUCT := $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/.metadata/sdk.json
BOARD := /ti/boards/.meta/CC3220S_LAUNCHXL

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = Board.obj main_nortos.obj fatsdraw_nortos.obj

NAME = fatsdraw

CFLAGS = -I.. \
    -I. \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source" \
    -DNORTOS_SUPPORT \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/nortos/posix" \
    --cpu=Cortex-M4 \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/display/lib/display.arm4" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/grlib/lib/iar/m4/grlib.a" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/third_party/spiffs/lib/iar/m4/spiffs.a" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_cc32xx.arm4" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/third_party/fatfs/lib/iar/m4/fatfs.a" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/nortos/lib/nortos_cc32xx.arm4" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/devices/cc32xx/driverlib/iar/Release/driverlib.a" \
    --config ../iar/CC3220S_LAUNCHXL_NoRTOS.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4 \
    --redirect _Printf=_PrintfSmall \
    --redirect _Scanf=_ScanfSmall \
    --semihosting=iar_breakpoint

all: $(NAME).out

Board.c: ../fatsdraw.syscfg
	@ echo Generating $@...
	@ $(NODE_JS) $(SYSCONFIG_TOOL) --product $(SYSCONFIG_PRODUCT) --board $(BOARD) --output $(@D) $<
Board.obj: Board.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

main_nortos.obj: ../main_nortos.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

fatsdraw_nortos.obj: ../fatsdraw_nortos.c Board.obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking...
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) Board.*
