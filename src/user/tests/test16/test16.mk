$(eval $(call clear-module-vars))
LOCAL_MODULE_PATH := $(call my-dir)

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := test16
LOCAL_PROCESS_SRC := test16-msg.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := proc16_1
LOCAL_PROCESS_SRC := proc16_1-msg.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := proc16_2
LOCAL_PROCESS_SRC := proc16_2-msg.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := proc16_3
LOCAL_PROCESS_SRC := proc16_3-msg.c
$(eval $(call build-test-process))

$(eval $(call build-test-module))
