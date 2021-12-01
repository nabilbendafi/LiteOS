include $(LITEOSTOPDIR)/config.mk
include $(LITEOSTOPDIR)/components/components.mk
include $(LITEOSTOPDIR)/demos/demos.mk

MODULE_NAME := $(LITEOS_PLATFORM)

ALL_ASSRCS := $(wildcard board/$(LITEOS_PLATFORM)/*.S)
ASSRCS := $(subst board/$(LITEOS_PLATFORM)/board.ld.S,,$(ALL_ASSRCS))

LOCAL_SRCS += $(ASSRCS)

USER_SRC = \
    $(LITEOSTOPDIR)/targets/bsp/drivers/$(HAL_DRIVER_TYPE)/Src/fsl_debug_console.c \
    $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/os_adapt/os_adapt.c \
    $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/Src/main.c \
    $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/Src/system_MK20D10.c \
    $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/Src/sys_init.c \
    $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/Src/usart.c \
    $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/Src/user_task.c
    C_SOURCES += $(USER_SRC)

ifeq ($(LOSCFG_COMPONENTS_FS_FATFS), y)
    ifeq ($(LOSCFG_FATFS_USE_SD_CARD), y)
        FS_SRC += \
            $(LITEOSTOPDIR)/targets/bsp/drivers/$(HAL_DRIVER_TYPE)/Src/fsl_sdhc.c
    endif
    ifeq ($(LOSCFG_FATFS_USE_SPI_FLASH), y)
        FS_SRC += \
            $(LITEOSTOPDIR)/targets/bsp/drivers/$(HAL_DRIVER_TYPE)/Src/fsl_dspi.c
    endif
    C_SOURCES += $(FS_SRC)
endif

# C includes
HAL_DRIVER_INC = \
   -I $(LITEOSTOPDIR)/targets/bsp/drivers/$(HAL_DRIVER_TYPE)/Inc \
   -I $(LITEOSTOPDIR)/targets/bsp/drivers/$(HAL_DRIVER_TYPE)/Inc/Legacy
    BOARD_INCLUDES += $(HAL_DRIVER_INC)

INCLUDE_INC = \
    -I $(LITEOSTOPDIR)/include
    BOARD_INCLUDES += $(INCLUDE_INC)

BSP_INC = \
    -I $(LITEOSTOPDIR)/kernel/extended/include \
    -I $(LITEOSTOPDIR)/kernel/include \
    -I $(LITEOSTOPDIR)/targets/bsp/common \
    -I $(LITEOSTOPDIR)/targets/bsp/include \
    -I $(LITEOSTOPDIR)/targets/bsp/hw/include \
    -I $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/include \

    BOARD_INCLUDES += $(BSP_INC)

USER_INC = \
    -I $(LITEOSTOPDIR)/targets/$(LITEOS_PLATFORM)/Inc
    BOARD_INCLUDES += $(USER_INC)

# C defines
C_DEFS +=  \
    -D USE_HAL_DRIVER \
    -D MK20D10 \
    -D CPU_MK20DX256VLK10 \
    -D SDK_DEBUGCONSOLE \
    -D DISABLE_WDOG \
    -D CLOCK_SETUP=0 \
    -D NDEBUG \
    -D __LITEOS__ \
    -D _ALL_SOURCE

ASM_SOURCES = \
    ${wildcard los_startup_gcc.S}

BOARD_SRCS += $(ASM_SOURCES)
BOARD_SRCS += $(C_SOURCES)

LOCAL_SRCS := $(subst $(CURDIR)/,, $(BOARD_SRCS))

BOARD_DEF += $(C_DEFS)
LOCAL_INCLUDE += $(BOARD_INCLUDES)

#LOCAL_CFLAGS += -Wno-unused-variable
#LOCAL_CFLAGS += -Wno-unused-const-variable

LOCAL_FLAGS := $(BOARD_DEF) $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)

include $(MODULE)
