DLKM_DIR := motorola/kernel/modules
LOCAL_PATH := $(call my-dir)

ifneq ($(FOCALTECH_TOUCH_IC_NAME),)
	KERNEL_CFLAGS += CONFIG_INPUT_FOCALTECH_0FLASH_MMI_IC_NAME=$(FOCALTECH_TOUCH_IC_NAME)
else
	KERNEL_CFLAGS += CONFIG_INPUT_FOCALTECH_0FLASH_MMI_IC_NAME=ft8726
endif

ifneq ($(BOARD_USES_PANEL_NOTIFICATIONS),)
	KERNEL_CFLAGS += CONFIG_INPUT_FOCALTECH_PANEL_NOTIFICATIONS=y
endif

include $(CLEAR_VARS)
LOCAL_MODULE := focaltech_0flash_v2_mmi.ko
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(KERNEL_MODULES_OUT)

include $(DLKM_DIR)/AndroidKernelModule.mk