$(eval $(call clear-module-vars))
LOCAL_MODULE_PATH := $(call my-dir)

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := test17
LOCAL_PROCESS_SRC := test17-msg.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := proc_return
LOCAL_PROCESS_SRC := proc_return.c
$(eval $(call build-test-process))

$(eval $(call build-test-module))
