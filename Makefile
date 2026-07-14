LOGUE_SDK_PATH ?= ../logue-sdk

SDK_ROOT := $(abspath $(LOGUE_SDK_PATH))
PROJECT_NAME := ximera_mki
DISTDIR ?= dist

NTS1_PLATFORM := nutekt-digital
MINILOGUE_XD_PLATFORM := minilogue-xd
PROLOGUE_PLATFORM := prologue

NTS1_MAKE := $(SDK_ROOT)/platform/$(NTS1_PLATFORM)/dummy-osc/Makefile
MINILOGUE_XD_MAKE := $(SDK_ROOT)/platform/$(MINILOGUE_XD_PLATFORM)/dummy-osc/Makefile
PROLOGUE_MAKE := $(SDK_ROOT)/platform/$(PROLOGUE_PLATFORM)/dummy-osc/Makefile

.PHONY: all install package package-all package-nts1 package-minilogue-xd package-prologue dist clean clean-all sdk-check

all: package

install package: package-nts1

package-all: package-nts1 package-minilogue-xd package-prologue

package-nts1: sdk-check
	$(MAKE) -f $(NTS1_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_ROOT)/platform/$(NTS1_PLATFORM) LDDIR=$(SDK_ROOT)/platform/$(NTS1_PLATFORM)/dummy-osc/ld BUILDDIR=$(CURDIR)/build/$(NTS1_PLATFORM) MANIFEST=manifests/$(NTS1_PLATFORM)/manifest.json install

package-minilogue-xd: sdk-check
	$(MAKE) -f $(MINILOGUE_XD_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_ROOT)/platform/$(MINILOGUE_XD_PLATFORM) LDDIR=$(SDK_ROOT)/platform/$(MINILOGUE_XD_PLATFORM)/dummy-osc/ld BUILDDIR=$(CURDIR)/build/$(MINILOGUE_XD_PLATFORM) MANIFEST=manifests/$(MINILOGUE_XD_PLATFORM)/manifest.json install

package-prologue: sdk-check
	$(MAKE) -f $(PROLOGUE_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_ROOT)/platform/$(PROLOGUE_PLATFORM) LDDIR=$(SDK_ROOT)/platform/$(PROLOGUE_PLATFORM)/dummy-osc/ld BUILDDIR=$(CURDIR)/build/$(PROLOGUE_PLATFORM) MANIFEST=manifests/$(PROLOGUE_PLATFORM)/manifest.json install

dist: package-all
	@mkdir -p $(DISTDIR)
	cp -f $(PROJECT_NAME).ntkdigunit $(DISTDIR)/$(PROJECT_NAME).ntkdigunit
	cp -f $(PROJECT_NAME).mnlgxdunit $(DISTDIR)/$(PROJECT_NAME).mnlgxdunit
	cp -f $(PROJECT_NAME).prlgunit $(DISTDIR)/$(PROJECT_NAME).prlgunit
	shasum -a 256 $(DISTDIR)/$(PROJECT_NAME).ntkdigunit $(DISTDIR)/$(PROJECT_NAME).mnlgxdunit $(DISTDIR)/$(PROJECT_NAME).prlgunit > $(DISTDIR)/SHA256SUMS.txt

clean: sdk-check
	$(MAKE) -f $(NTS1_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_ROOT)/platform/$(NTS1_PLATFORM) LDDIR=$(SDK_ROOT)/platform/$(NTS1_PLATFORM)/dummy-osc/ld BUILDDIR=$(CURDIR)/build/$(NTS1_PLATFORM) MANIFEST=manifests/$(NTS1_PLATFORM)/manifest.json clean

clean-all: sdk-check
	$(MAKE) -f $(NTS1_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_ROOT)/platform/$(NTS1_PLATFORM) LDDIR=$(SDK_ROOT)/platform/$(NTS1_PLATFORM)/dummy-osc/ld BUILDDIR=$(CURDIR)/build/$(NTS1_PLATFORM) MANIFEST=manifests/$(NTS1_PLATFORM)/manifest.json clean
	$(MAKE) -f $(MINILOGUE_XD_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_ROOT)/platform/$(MINILOGUE_XD_PLATFORM) LDDIR=$(SDK_ROOT)/platform/$(MINILOGUE_XD_PLATFORM)/dummy-osc/ld BUILDDIR=$(CURDIR)/build/$(MINILOGUE_XD_PLATFORM) MANIFEST=manifests/$(MINILOGUE_XD_PLATFORM)/manifest.json clean
	$(MAKE) -f $(PROLOGUE_MAKE) PROJECTDIR=$(CURDIR) PLATFORMDIR=$(SDK_ROOT)/platform/$(PROLOGUE_PLATFORM) LDDIR=$(SDK_ROOT)/platform/$(PROLOGUE_PLATFORM)/dummy-osc/ld BUILDDIR=$(CURDIR)/build/$(PROLOGUE_PLATFORM) MANIFEST=manifests/$(PROLOGUE_PLATFORM)/manifest.json clean

sdk-check:
	@test -f "$(NTS1_MAKE)" || (echo "Set LOGUE_SDK_PATH to a checked-out korginc/logue-sdk path"; exit 1)
	@test -f "$(MINILOGUE_XD_MAKE)" || (echo "Missing minilogue-xd SDK files under LOGUE_SDK_PATH"; exit 1)
	@test -f "$(PROLOGUE_MAKE)" || (echo "Missing prologue SDK files under LOGUE_SDK_PATH"; exit 1)
