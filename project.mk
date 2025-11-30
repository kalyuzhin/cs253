ifneq (,$(wildcard .env))
	include .env
endif

CUR_DIR=$(shell pwd)
LOCAL_BIN_FOLDER=$(CUR_DIR)/bin
MAIN_PATH=$(CUR_DIR)/main.cpp

PLATFORM:=$(shell bash scripts/os_script.sh)

ifeq ($(PLATFORM), windows)
    COMPILER = cl
    BUILD_FILE = $(LOCAL_BIN_FOLDER)/run.exe
    MKDIR_CMD = if not exist "$(LOCAL_BIN_FOLDER)" mkdir "$(subst /,\\,$(LOCAL_BIN_FOLDER))"
else ifeq ($(PLATFORM), macOS)
    COMPILER = g++
    CPPFLAGS = -Wall -std=c++20 -O3 -o
    BUILD_FILE = $(LOCAL_BIN_FOLDER)/run
    MKDIR_CMD = mkdir -p $(LOCAL_BIN_FOLDER)
else ifeq ($(PLATFORM), linux)
    COMPILER = g++
    CPPFLAGS = -Wall -std=c++20 -O3 -o
    BUILD_FILE = $(LOCAL_BIN_FOLDER)/run
    MKDIR_CMD = mkdir -p $(LOCAL_BIN_FOLDER)
else
    $(error Unsupported platform: $(PLATFORM))
endif




.PHONY: help
## prints help about all targets
help:
	@echo ""
	@echo "Usage:"
	@echo "  make <target>"
	@echo ""
	@echo "Targets:"
	@awk '                                \
		BEGIN { comment=""; }             \
		/^\s*##/ {                         \
		    comment = substr($$0, index($$0,$$2)); next; \
		}                                  \
		/^[a-zA-Z0-9_-]+:/ {               \
		    target = $$1;                  \
		    sub(":", "", target);          \
		    if (comment != "") {           \
		        printf "  %-17s %s\n", target, comment; \
		        comment="";                \
		    }                              \
		}' $(MAKEFILE_LIST)
	@echo ""

.PHONY: build
## build project
build:
	$(shell MKDIR_CMD)
	$(COMPILER) $(CPPFLAGS) $(BUILD_FILE) main.cpp labs/lab2.cpp
.PHONY: run
## run project
run: build
	$(BUILD_FILE)

.PHONY: archive
## make zip-archive of the project
archive:
	zip -r task.zip . -x "bin/*" "cmake-build-debug/*" "cmake-build-release/*" ".idea/*" ".git/*" ".DS_Store" ".env"
