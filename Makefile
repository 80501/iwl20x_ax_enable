VERSION         := 1.0.1
TARGET          := $(shell uname -r)
DKMS_ROOT_PATH  := /usr/src/iwl20x_ax_en-$(VERSION)

KERNEL_MODULES	:= /lib/modules/$(TARGET)

ifneq ("","$(wildcard /usr/src/linux-headers-$(TARGET)/*)")
# Ubuntu
KERNEL_BUILD	:= /usr/src/linux-headers-$(TARGET)
else
ifneq ("","$(wildcard /usr/src/kernels/$(TARGET)/*)")
# Fedora
KERNEL_BUILD	:= /usr/src/kernels/$(TARGET)
else
KERNEL_BUILD	:= $(KERNEL_MODULES)/build
endif
endif

obj-m	:= $(patsubst %,%.o,iwl20x_ax_en)
obj-ko	:= $(patsubst %,%.ko,iwl20x_ax_en)

.PHONY: all modules clean dkms-install dkms-install-swapped dkms-uninstall

all: modules

modules:
	@$(MAKE) -C $(KERNEL_BUILD) M=$(CURDIR) modules

clean:
	@$(MAKE) -C $(KERNEL_BUILD) M=$(CURDIR) clean

dkms-install:
	mkdir $(DKMS_ROOT_PATH)
	cp $(CURDIR)/dkms.conf $(DKMS_ROOT_PATH)
	cp $(CURDIR)/Makefile $(DKMS_ROOT_PATH)
	cp $(CURDIR)/iwl20x_ax_en.c $(DKMS_ROOT_PATH)

	sed -e "s/@CFLGS@/${MCFLAGS}/" \
	    -e "s/@VERSION@/$(VERSION)/" \
	    -i $(DKMS_ROOT_PATH)/dkms.conf

	dkms add iwl20x_ax_en/$(VERSION)
	dkms build iwl20x_ax_en/$(VERSION)
	dkms install iwl20x_ax_en/$(VERSION)

dkms-uninstall:
	dkms remove iwl20x_ax_en/$(VERSION) --all
	rm -rf $(DKMS_ROOT_PATH)
