#
# Ensimag - Projet système
# Copyright (C) 2012 - Damien Dejean <dam.dejean@gmail.com>
#

### Check requirements ###
ifndef TESTS_DIR
    $(error Tests source directory undefined \(TESTS_DIR undefined\).)
else
ifeq ("$(TESTS_DIR)", "")
    $(error Tests source directory undefined \(TESTS_DIR is empty\).)
endif
endif
ifndef ULIB
    $(error Ulib path undefined \(ULIB undefined\).)
else
ifeq ("$(ULIB)", "")
    $(error Ulib path undefined \(ULIB is empty\).)
endif
endif
ifndef TESTS_OPTIONS
    $(error Tests options undefined \(TESTS_OPTIONS undefined\).)
else
ifeq ("$(TESTS_OPTIONS)", "")
    $(error Tests options undefined \(TESTS_OPTIONS is empty\).)
endif
endif


### Tests configuration ###
$(info Building test suite with following options: $(TESTS_OPTIONS))


### Compile a self contain standard library to be "student safe" ###
TESTS_LIB_DIR     := $(TESTS_DIR)/tlib
TESTS_LIB_SRC     := $(wildcard $(TESTS_LIB_DIR)/*.c)
TESTS_LIB_OBJS    := $(patsubst %.c,%.o,$(addprefix $(OUTPUT)/, $(TESTS_LIB_SRC)))
TESTS_LIB_CFLAGS  := $(addprefix -D,$(TESTS_OPTIONS)) \
-Wno-builtin-declaration-mismatch
TESTS_LIB_ARFLAGS := rcs

$(OUTPUT)/$(TESTS_LIB_DIR)/%.o: $(TESTS_LIB_DIR)/%.c | $(OUTPUT)/$(TESTS_LIB_DIR)
	$(call cmd,CC TLIB $<, $(dir $@), \
	$(CC) $(CFLAGS) $(TESTS_LIB_CFLAGS) -I$(TESTS_LIB_DIR) -MMD -MP -c $^ -o $@)

$(OUTPUT)/$(TESTS_DIR)/tlib.a: $(TESTS_LIB_OBJS) | $(OUTPUT)/$(TESTS_DIR)
	$(call cmd,AS TLIB $<, $(dir $@), \
	$(AR) $(TESTS_LIB_ARFLAGS) $@ $^)

$(OUTPUT)/$(TESTS_DIR) $(OUTPUT)/$(TESTS_LIB_DIR):
	@mkdir -p $@
	@echo \# Build tlib \#

### Compile each tests as a couple of processes ###
TESTS_MAKEFILES := $(shell find $(TESTS_DIR)/ -name "*.mk")
TESTS_CFLAGS    := $(filter-out -Wall, $(CFLAGS)) -Waddress  -Waggregate-return \
	-Warray-bounds -Wno-attributes -Wno-builtin-macro-redefined -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Wclobbered -Wcomment -Wconversion \
	-Wcoverage-mismatch -Wno-deprecated -Wno-deprecated-declarations \
	-Wdisabled-optimization -Wno-div-by-zero -Wempty-body \
	-Wno-endif-labels -Werror -Wfloat-equal -Wformat -Wformat=2 \
	-Wno-format-contains-nul -Wno-format-extra-args -Wformat-nonliteral \
	-Wformat-security -Wformat-y2k -Wignored-qualifiers \
	-Wimplicit -Wimplicit-function-declaration  -Wimplicit-int -Winit-self \
	-Winline -Wno-int-to-pointer-cast -Winvalid-pch -Wunsafe-loop-optimizations \
	-Wlogical-op -Wmissing-braces -Wmissing-field-initializers \
	-Wmissing-format-attribute -Wmissing-include-dirs \
	-Wno-multichar -Wnonnull -Wno-overflow -Woverlength-strings -Wpacked \
	-Wpacked-bitfield-compat -Wpadded -Wparentheses -Wpointer-arith -Wno-builtin-declaration-mismatch \
	-Wno-pointer-to-int-cast -Wredundant-decls -Wreturn-type -Wsequence-point \
	-Wshadow -Wsign-compare -Wsign-conversion -Wstack-protector \
	-Wstrict-overflow -Wswitch -Wswitch-default -Wswitch-enum -Wsync-nand \
	-Wsystem-headers -Wtrigraphs -Wtype-limits -Wundef \
	-Wuninitialized -Wunknown-pragmas -Wno-pragmas \
	-Wunused -Wunused-function -Wunused-label -Wunused-parameter \
	-Wunused-value -Wunused-variable -Wunused-but-set-parameter \
	-Wunused-but-set-variable -Wvariadic-macros -Wvla -Wvolatile-register-var \
	-Wwrite-strings \
	$(addprefix -D,$(TESTS_OPTIONS))
TESTS_LDFLAGS   := -nostdlib -T build/apps.lds
TESTS_TARGETS   :=

# A way to clean all module variables before using them
define clear-module-vars
LOCAL_MODULE_PATH:=
endef

# A way to clean test variables before using them
define clear-test-vars
LOCAL_TEST_NAME:=
LOCAL_TEST_SRC:=
endef

# Let the submakefiles declare a test process
define build-test-process
PRIVATE_SRCS   := $$(addprefix $$(LOCAL_MODULE_PATH)/,$$(LOCAL_PROCESS_SRC))
PRIVATE_OUT    := $$(addprefix $$(OUTPUT)/,$$(LOCAL_MODULE_PATH))
PRIVATE_OBJS   := $$(addprefix $$(OUTPUT)/,$$(patsubst %.c,%.o,$$(PRIVATE_SRCS)))
PRIVATE_BIN    := $$(OUTPUT)/$$(LOCAL_PROCESS_NAME).bin
PRIVATE_INC    := $$(addprefix -I,$$(LOCAL_MODULE_PATH) $$(TESTS_LIB_DIR))
PRIVATE_CFLAGS := $$(TESTS_CFLAGS) $$(strip $$(LOCAL_CFLAGS))

$$(PRIVATE_BIN): $$(PRIVATE_OBJS) $$(OUTPUT)/$$(TESTS_DIR)/tlib.a $$(ULIB)
	$(call cmd,LD TEST -> $$(@:../%.bin=%.elf), $$(dir $$@), \
	$$(LD) $$(TESTS_LDFLAGS) -o $$(@:%.bin=%.elf) $$^)
	$(call cmd,OBJCOPY TEST -> $$(@:../%=%), $$(dir $$@), \
    	$(OBJCOPY) -O binary $$(@:%.bin=%.elf) $$@ )
#	$$(LD) $$(filter-out --oformat=binary,$$(TESTS_LDFLAGS)) -o $$(patsubst %.bin,%.debug,$$@) $$^

$$(PRIVATE_OUT)/%.o: $$(LOCAL_MODULE_PATH)/%.c | $$(PRIVATE_OUT)
	$$(call cmd,CC TEST $$<, $$(dir $$@), \
	$$(CC) $$(PRIVATE_CFLAGS) $$(PRIVATE_INC) -c $$< -o $$@ -MMD -MP)

# Add ourself to the common tests target
TESTS_TARGETS += $$(PRIVATE_BIN)
endef

# Test module conclusion
define build-test-module
PRIVATE_OUT := $$(addprefix $$(OUTPUT)/,$$(LOCAL_MODULE_PATH))

$$(PRIVATE_OUT):
	@mkdir -p $$@
	@echo \# Build test $$(@:$$(OUTPUT)/tests/%=%)
endef

### Finally include all sub makefiles ###
include $(TESTS_MAKEFILES)
