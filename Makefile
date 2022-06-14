KBUILD_LDFLAGS_MODULE += --strip-debug
obj-m+= iwl20x_ax_en.o

KDIR= /lib/modules/$(shell uname -r)/build
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules 
