$(eval $(call clear-module-vars))
LOCAL_MODULE_PATH := $(call my-dir)

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := test15
LOCAL_PROCESS_SRC := test15-msg.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := pmsg1
LOCAL_PROCESS_SRC := pmsg1.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := pmsg2
LOCAL_PROCESS_SRC := pmsg2.c
$(eval $(call build-test-process))

$(eval $(call build-test-module))
