LOGUE_SDK_PATH ?= ../logue-sdk

SDK_PLATFORM := $(abspath $(LOGUE_SDK_PATH))/platform/nutekt-digital
SDK_OSC_MAKE := $(SDK_PLATFORM)/dummy-osc/Makefile
SDK_OSC_LD := $(SDK_PLATFORM)/dummy-osc/ld

.PHONY: all install package clean sdk-check

all: sdk-check
	$(MAKE) -f $(SDK_OSC_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_PLATFORM) LDDIR=$(SDK_OSC_LD) all

install package: sdk-check
	$(MAKE) -f $(SDK_OSC_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_PLATFORM) LDDIR=$(SDK_OSC_LD) install

clean: sdk-check
	$(MAKE) -f $(SDK_OSC_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_PLATFORM) LDDIR=$(SDK_OSC_LD) clean

sdk-check:
	@test -f "$(SDK_OSC_MAKE)" || (echo "Set LOGUE_SDK_PATH to a checked-out korginc/logue-sdk path"; exit 1)
