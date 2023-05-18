DLKM_DIR := motorola/kernel/modules
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := cw2217b_fg_mmi.ko
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(KERNEL_MODULES_OUT)

ifeq ($(DYNAMIC_UPDATE_UI_FULL),true)
	KERNEL_CFLAGS += CONFIG_DYNAMIC_UPDATE_UI_FULL=y
	KBUILD_OPTIONS += CONFIG_DYNAMIC_UPDATE_UI_FULL=y
endif
KBUILD_OPTIONS_GKI += GKI_OBJ_MODULE_DIR=gki

include $(DLKM_DIR)/AndroidKernelModule.mk
