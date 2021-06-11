MXVER:=1.0.1
KERVER    := $(shell uname -r)
KERNELDIR := /lib/modules/$(KERVER)/build
PWD       := $(shell pwd)
TOPDIR    := $(PWD)

ifeq ($(KERNELRELEASE),)

all: clean modules

modules clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@ TOPDIR=$(TOPDIR) 
	
install: modules modules_install

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $@ TOPDIR=$(TOPDIR)
	@depmod -a
	@echo "**********************************************************************"
	@echo " MOXA CAN Family Device Driver ver $(MXVER)"
	@echo " Installed successfully."
	@echo "**********************************************************************"

uninstall:
	@rm -f /lib/modules/$(shell uname -r)/extra/mxcan-dev.ko
	@rm -f /lib/modules/$(shell uname -r)/extra/mxsja1000.ko
	@rm -f /lib/modules/$(shell uname -r)/extra/mxcandrv.ko
	@depmod -a
	@echo "**********************************************************************"
	@echo " MOXA CAN Family Device Driver ver $(MXVER)"
	@echo " Removed successfully."
	@echo "**********************************************************************"
	
else

ifneq ($(KBUILD_EXTMOD),)

ifneq ($(CONFIG_CAN_CALC_BITTIMING),n)
EXTRA_CFLAGS += -DCONFIG_CAN_CALC_BITTIMING
endif

ifeq ($(CONFIG_CAN_DEBUG_DEVICES),y)
EXTRA_CFLAGS += -DCONFIG_CAN_DEBUG_DEVICES -DDEBUG
endif

endif

obj-m += mxcan-dev.o
mxcan-dev-y := mxdev.o
obj-m += mxsja1000.o
obj-m += mxcandrv.o

endif
