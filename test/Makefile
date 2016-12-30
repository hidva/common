project_path := $(shell pwd)
common_project_path := $(shell cd $(project_path)/.. && pwd)


BIN := run_test

C_SRC := 
CXX_SRC :=  $(common_project_path)/test/$(BIN).cc

CXX_SRC +=  $(common_project_path)/src/rrid/scope_exit_test.cc	\
	$(common_project_path)/src/rrid/on_exception_test.cc

CXX_SRC += $(common_project_path)/src/hex_coding/hex_coding_test.cc	\
	$(common_project_path)/src/hex_coding/hex_coding.cc

CXX_SRC += $(common_project_path)/src/common/inline_utils_test.cc	\
	$(common_project_path)/src/common/utils_test.cc	\
	$(common_project_path)/src/common/lru_test.cc	\
	$(common_project_path)/src/common/utils.cc	
	
CXX_SRC += $(common_project_path)/src/concurrent/mutex_test.cc


CXX_SRC += $(common_project_path)/src/execute_time_get/execute_time_get_test.cc

CXX_SRC += $(common_project_path)/src/glibc_cxx_wrap/test_2.cc \
	$(common_project_path)/src/glibc_cxx_wrap/test_1.cc	\
	$(common_project_path)/src/glibc_cxx_wrap/14_file_system_interface_test.cc	\
	$(common_project_path)/src/glibc_cxx_wrap/13_low_level_io.cc	\
	$(common_project_path)/src/glibc_cxx_wrap/13_low_level_io_test.cc

CXX_SRC += $(common_project_path)/src/log/pp_qq_log.cc \
	$(common_project_path)/src/log/pp_qq_log_test.cc

CXX_SRC += $(common_project_path)/src/exception/errno_exception.cc \
	$(common_project_path)/src/exception/resource_exception.cc	\
	$(common_project_path)/src/exception/errno_exception_test.cc

CXX_SRC += $(common_project_path)/src/formatter/stdio_formatter_test.cc

CXX_SRC += $(common_project_path)/src/string/extended_std_string_test.cc	\
	$(common_project_path)/src/string/string_piece.cc	\
	$(common_project_path)/src/string/ctype.cc		\
	$(common_project_path)/src/string/ctype_test.cc		\
	$(common_project_path)/src/string/kmpdfa.cc		\
	$(common_project_path)/src/string/kmpdfa_test.cc	
#	$(common_project_path)/src/string/string_piece_test.cc

# CXX_SRC += $(common_project_path)/src/cipher/counter.cc $(common_project_path)/src/cipher/counter_test.cc	\
#	$(common_project_path)/src/cipher/block_cipher.cc \
#    $(common_project_path)/src/cipher/memxor.cc $(common_project_path)/src/cipher/memxor_test.cc \
#    $(common_project_path)/src/cipher/aes_cipher.cc $(common_project_path)/src/cipher/aes_cipher_test.cc	\
#	$(common_project_path)/src/cipher/aes256_ctr_test.cc $(common_project_path)/src/cipher/ctr_cipher_mode.cc

#CXX_SRC += $(common_project_path)/src/digest/digest.cc $(common_project_path)/src/digest/sha3_256.cc	\
#	$(common_project_path)/src/digest/sha3_256_test.cc

C_CXX_SHARED_CFLAGS := -Wall -O0 -g -pthread -Wfatal-errors

CFLAGS := 
CXXFLAGS := 
	
LDFLAGS := 	




LDFLAGS := -pthread



### gtest
gtest_install_prefix := $(HOME)/lib/googletest
C_CXX_SHARED_CFLAGS += -I$(gtest_install_prefix)/include

C_CXX_SHARED_CFLAGS += -I$(common_project_path)/src/
LDFLAGS += -L$(gtest_install_prefix)/lib -lgtest


CFLAGS := $(C_CXX_SHARED_CFLAGS)

CXXFLAGS := -std=gnu++11 $(C_CXX_SHARED_CFLAGS)



### 以下不需要配置 ###
all: 

###

GCC := gcc
GXX := g++

OBJ_DIR := objs
DEP_DIR := $(OBJ_DIR)/deps
BIN_DIR := bin

C_OBJS := $(C_SRC:.c=.c.o)
CXX_OBJS := $(foreach ccfile,$(CXX_SRC),$(ccfile).o)
ALL_DEPS := $(C_OBJS:.o=.dep)
ALL_DEPS += $(CXX_OBJS:.o=.dep)

C_OBJS := $(foreach var,$(C_OBJS),$(OBJ_DIR)/$(var))
CXX_OBJS := $(foreach var,$(CXX_OBJS),$(OBJ_DIR)/$(var))
ALL_DEPS := $(foreach var,$(ALL_DEPS),$(DEP_DIR)/$(var))

all: $(BIN_DIR)/$(BIN)

include $(ALL_DEPS)

$(DEP_DIR)/%.c.dep: %.c
	@if [ ! -d $(dir $@) ] ; then mkdir -p $(dir $@); fi
	@echo [依赖生成]: $< --- $@
	@$(GCC) -E -M -MQ $(OBJ_DIR)/$*.c.o -MQ $@ -MF $@ $< $(CFLAGS)

$(DEP_DIR)/%.cc.dep: %.cc
	@if [ ! -d $(dir $@) ] ; then mkdir -p $(dir $@); fi
	@echo [依赖生成]: $< --- $@
	@$(GXX) -E -M -MQ $(OBJ_DIR)/$*.cc.o -MQ $@ -MF $@ $< $(CXXFLAGS)

$(DEP_DIR)/%.cpp.dep: %.cpp
	@if [ ! -d $(dir $@) ] ; then mkdir -p $(dir $@); fi
	@echo [依赖生成]: $< --- $@
	@$(GXX) -E -M -MQ $(OBJ_DIR)/$*.cpp.o -MQ $@ -MF $@ $< $(CXXFLAGS)
	
	
$(OBJ_DIR)/%.c.o: %.c
	@if [ ! -d $(dir $@) ] ; then mkdir -p $(dir $@); fi
	@echo [编译]: $< --- $@
	@$(GCC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/%.cc.o: %.cc
	@if [ ! -d $(dir $@) ] ; then mkdir -p $(dir $@); fi
	@echo [编译]: $< --- $@
	@$(GXX) -c $< -o $@ $(CXXFLAGS)

$(OBJ_DIR)/%.cpp.o: %.cpp
	@if [ ! -d $(dir $@) ] ; then mkdir -p $(dir $@); fi
	@echo [编译]: $< --- $@
	@$(GXX) -c $< -o $@ $(CXXFLAGS)
	
$(BIN_DIR)/$(BIN): $(C_OBJS) $(CXX_OBJS)
	@if [ ! -d $(BIN_DIR) ] ; then mkdir -p $(BIN_DIR); fi
	@echo [链接]: $^ --- $@
ifeq ($(strip $(CXX_SRC)),)
	@$(GCC) -o $(BIN_DIR)/$(BIN) $(C_OBJS) $(CXX_OBJS) $(LDFLAGS)
else
	@$(GXX) -o $(BIN_DIR)/$(BIN) $(C_OBJS) $(CXX_OBJS) $(LDFLAGS)
endif

clean:
	-@rm -rv $(BIN_DIR) $(OBJ_DIR)
	
.PHONY: all clean 
	


