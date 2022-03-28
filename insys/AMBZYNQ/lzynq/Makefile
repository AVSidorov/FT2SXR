
ifneq   "$(findstring 2.4.,$(shell uname -a))" ""

all:
	make -f Makefile-2.4

clean:
	make -f Makefile-2.4 clean

install:
	make -f Makefile-2.4 install

else

KERNELVER := $(shell uname -r)

DIR0 = $(PWD)/../../BRDINC
DIR1 = $(PWD)/../../WDMLIBS/linux
DIR2 = $(PWD)/../../BRDLIBS
DIR3 = $(PWD)/..

DIRS = -I$(DIR0) -I$(DIR1) -I$(DIR2) -I$(DIR3)

ifneq ($(ARCH),)
EXTRA_CFLAGS += -O2 $(DIRS) -D$(ARCH)
else
EXTRA_CFLAGS += -O2 $(DIRS)
endif

EXT := ../../WDMLIBS/linux/event.c ../../WDMLIBS/linux/dmachan.c \
../../WDMLIBS/linux/memhelp.c ../../WDMLIBS/linux/pagehelp.c
SOURCE := $(EXT)
OBJ_FILES := $(SOURCE:.c=.o)


ifneq ($(KERNELRELEASE),)

obj-m := lzynq.o
lzynq-y := zynqdev.o
lzynq-y += $(OBJ_FILES) start.o devproc.o system.o\
 hardware.o ioctlrw.o isr.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" modules
#	cp *.ko ../../bin
#	cp *.ko /tftpboot/nfsroot-fmc130e/home/embedded/drivers
	cp *.ko /tftpboot

endif

distclean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.bak .*.cache *.d $(OBJ_FILES)
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.bak .*.cache *.d *.markers *.symvers *.order .cache.mk *-safe $(OBJ_FILES)

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend

ifeq (.depend, $(wildcard .depend))
include .depend
endif

ifeq "$(INSTALL_PREFIX)" ""
install:
	chmod +x $(PWD)/insert
	$(PWD)/insert dbg_trace=0 err_trace=1
else
install:
	cp ambpex.ko $(INSTALL_PREFIX)/home/$(TARGETFS_USER)/drivers
endif

endif
