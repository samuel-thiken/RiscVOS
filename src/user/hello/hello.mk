$(eval $(call clear-module-vars))
LOCAL_MODULE_PATH := $(call my-dir)

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := hello
LOCAL_PROCESS_SRC := hello.c
$(eval $(call build-test-process))
$(eval $(call build-test-module))
