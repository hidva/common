BIN := run_test

PROJECT_PATH := /home/pp-qq/project/org/pp-qq/common/

CXX_SRC :=  $(PROJECT_PATH)/test/$(BIN).cc

CXX_SRC +=  $(PROJECT_PATH)/src/rrid/scope_exit_test.cc	\
	    $(PROJECT_PATH)/src/rrid/on_exception_test.cc

CXX_SRC += $(PROJECT_PATH)/src/hex_coding/hex_coding_test.cc	\
	   $(PROJECT_PATH)/src/hex_coding/hex_coding.cc

CXX_SRC += $(PROJECT_PATH)/src/common/inline_utils_test.cc

CXX_SRC += $(PROJECT_PATH)/src/execute_time_get/execute_time_get_test.cc

CXX_SRC += $(PROJECT_PATH)/src/glibc_cxx_wrap/test_2.cc \
	   $(PROJECT_PATH)/src/glibc_cxx_wrap/test_1.cc	\
	   $(PROJECT_PATH)/src/glibc_cxx_wrap/14_file_system_interface_test.cc	\
	   $(PROJECT_PATH)/src/glibc_cxx_wrap/13_low_level_io.cc	\
	   $(PROJECT_PATH)/src/glibc_cxx_wrap/13_low_level_io_test.cc

CXX_SRC += $(PROJECT_PATH)/src/log/pp_qq_log.cc \
	   $(PROJECT_PATH)/src/log/pp_qq_log_test.cc

CXX_SRC += $(PROJECT_PATH)/src/exception/errno_exception.cc \
	   $(PROJECT_PATH)/src/exception/errno_exception_test.cc

CXX_SRC += $(PROJECT_PATH)/src/formatter/stdio_formatter_test.cc

CXX_SRC += $(PROJECT_PATH)/src/string/extended_std_string_test.cc	\
	   $(PROJECT_PATH)/src/string/string_piece.cc	\
	   $(PROJECT_PATH)/src/string/string_piece_test.cc

CXX_SRC += $(PROJECT_PATH)/src/cipher/counter.cc $(PROJECT_PATH)/src/cipher/counter_test.cc	\
	$(PROJECT_PATH)/src/cipher/block_cipher.cc \
    $(PROJECT_PATH)/src/cipher/memxor.cc $(PROJECT_PATH)/src/cipher/memxor_test.cc \
    $(PROJECT_PATH)/src/cipher/aes_cipher.cc $(PROJECT_PATH)/src/cipher/aes_cipher_test.cc	\
	$(PROJECT_PATH)/src/cipher/aes256_ctr_test.cc $(PROJECT_PATH)/src/cipher/ctr_cipher_mode.cc

CXX_SRC += $(PROJECT_PATH)/src/digest/digest.cc $(PROJECT_PATH)/src/digest/sha3_256.cc	\
	$(PROJECT_PATH)/src/digest/sha3_256_test.cc

C_CXX_SHARED_CFLAGS := -Wall -O0 -g -pthread

# gtest 编译配置
gtest_install_prefix := /home/pp-qq/lib/gtest
C_CXX_SHARED_CFLAGS += -I$(gtest_install_prefix)/include

C_CXX_SHARED_CFLAGS += -I$(PROJECT_PATH)/src/


LDFLAGS := -pthread

# nettle
#C_CXX_SHARED_CFLAGS += -I/home/pp-qq/lib/nettle/include
#LDFLAGS += -L/home/pp-qq/lib/nettle/lib -lnettle
LDFLAGS += -lnettle

# openssl
#LDFLAGS += -lcrypto

# gtest 链接配置

LDFLAGS += -L$(gtest_install_prefix)/lib -lgtest


CFLAGS := $(C_CXX_SHARED_CFLAGS)

CXXFLAGS := -std=gnu++11 $(C_CXX_SHARED_CFLAGS)

# 安装路径的前缀.
PREFIX := /tmp/the_path_should_not_be_exists

####### 以下不需要配置 ###############
OBJ_DIR := objs
DEP_DIR := $(OBJ_DIR)/deps
BIN_DIR := bin
DISTDIR := $(OBJ_DIR)/$(TARNAME).tar.gz.tmp.dir
CWD_FULLPATH := $(shell pwd)

C_OBJS := $(C_SRC:.c=.c.o)
CXX_OBJS := $(CXX_SRC:.cc=.cc.o)
ALL_DEPS := $(C_OBJS:.o=.dep)
ALL_DEPS += $(CXX_OBJS:.o=.dep)

C_OBJS := $(foreach var,$(C_OBJS),$(OBJ_DIR)/$(var))
CXX_OBJS := $(foreach var,$(CXX_OBJS),$(OBJ_DIR)/$(var))
ALL_DEPS := $(foreach var,$(ALL_DEPS),$(DEP_DIR)/$(var))

all: $(BIN_DIR)/$(BIN)


include $(ALL_DEPS)

$(DEP_DIR)/%.c.dep: %.c
	@if [ ! -d $(dir $@) ] ; then mkdir -pv $(dir $@); fi
	gcc -E -M -MQ $(OBJ_DIR)/$*.c.o -MQ $@ -MF $@ $< $(CFLAGS)

$(DEP_DIR)/%.cc.dep: %.cc
	@if [ ! -d $(dir $@) ] ; then mkdir -pv $(dir $@); fi
	gcc -E -M -MQ $(OBJ_DIR)/$*.cc.o -MQ $@ -MF $@ $< $(CXXFLAGS)

$(OBJ_DIR)/%.c.o: %.c
	@if [ ! -d $(dir $@) ] ; then mkdir -pv $(dir $@); fi
	gcc -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/%.cc.o: %.cc
	@if [ ! -d $(dir $@) ] ; then mkdir -pv $(dir $@); fi
	g++ -c $< -o $@ $(CXXFLAGS)

$(BIN_DIR)/$(BIN): $(C_OBJS) $(CXX_OBJS)
	@if [ ! -d $(BIN_DIR) ] ; then mkdir -pv $(BIN_DIR); fi
ifeq ($(strip $(CXX_SRC)),)
	gcc -o $(BIN_DIR)/$(BIN) $(C_OBJS) $(CXX_OBJS) $(LDFLAGS)
else
	g++ -o $(BIN_DIR)/$(BIN) $(C_OBJS) $(CXX_OBJS) $(LDFLAGS)
endif

clean:
	-@rm -v  $(ALL_DEPS) $(C_OBJS) $(CXX_OBJS) $(BIN_DIR)/$(BIN)

install:
	@mkdir -pv $(PREFIX)/bin
	@cp $(BIN_DIR)/$(BIN) $(PREFIX)/bin -v
	@ln -svf $(PREFIX)/bin/$(BIN)  $(PREFIX)/bin/sym

.PHONY: all clean mkObjDir mkDepDir mkBinDir install $(DISTDIR)


dist: $(TARNAME).tar.gz

$(TARNAME).tar.gz: $(DISTDIR)
	cd $(DISTDIR); \
	tar czvf $(CWD_FULLPATH)/$(TARNAME).tar.gz .
	rm -rf $(DISTDIR)

$(DISTDIR):
	mkdir -p $(DISTDIR)
	git ls-files | xargs -L 1 dirname | sed -e 's|^|$(DISTDIR)/|' | xargs -L 1 mkdir -p
	git ls-files | sed -e 's|\(.*\)|\0 $(DISTDIR)/\0|' | xargs -L 1 cp
	-rm -f $(DISTDIR)/.git*
