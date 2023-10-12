$(eval $(call clear-module-vars))
LOCAL_MODULE_PATH := $(call my-dir)

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := test14
LOCAL_PROCESS_SRC := test14-msg.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := psender1
LOCAL_PROCESS_SRC := psender1-msg.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := psender2
LOCAL_PROCESS_SRC := psender2-msg.c
$(eval $(call build-test-process))

$(eval $(call build-test-module))
