
LIBNAME  := libbzynq.so
SONAME   := $(LIBNAME).0

include $(BARDYDIR)/make/macros.mk

DIRS := $(GIPCYDIR)/include $(BARDYDIR)/BRDINC $(BARDYDIR)/BRDINC/ctrladmpro $(BARDYDIR)/icrinc \
$(BARDYDIR)/BRDLIBS $(BARDYDIR)/BRDLIBS/BASESRV $(BARDYDIR)/BRDLIBS/SDRAMSRV $(BARDYDIR)/AMBZYNQ $(BARDYDIR)/AMBZYNQ/bzynq
INC := $(addprefix -I, $(DIRS))

EXTFILES := $(BARDYDIR)/BRDLIBS/streamsrv.cpp \
$(BARDYDIR)/BRDLIBS/module.cpp \
$(BARDYDIR)/BRDLIBS/basemodule.cpp \
$(BARDYDIR)/BRDLIBS/service.cpp \
$(BARDYDIR)/BRDLIBS/adm2if.cpp \
$(BARDYDIR)/BRDLIBS/useful.cpp \
$(BARDYDIR)/BRDLIBS/BASESRV/testsrv.cpp \
$(BARDYDIR)/BRDLIBS/BASESRV/sysmonsrv.cpp \
$(BARDYDIR)/BRDLIBS/SDRAMSRV/sdramsrv.cpp \
$(BARDYDIR)/BRDLIBS/SDRAMSRV/sdramambpcdsrv.cpp \
$(BARDYDIR)/BRDLIBS/SDRAMSRV/ddr3sdramsrv.cpp \
$(BARDYDIR)/BRDLIBS/SDRAMSRV/ddr4sdramsrv.cpp \
$(BARDYDIR)/AMBZYNQ/basedllini.cpp \
$(BARDYDIR)/AMBZYNQ/baseentry.cpp

SRC := $(wildcard *.cpp)
SRC += $(EXTFILES)
SOURCE += $(SRC)
OBJ_FILES := $(notdir $(SOURCE:.cpp=.o))

CFLAGS += $(INC)
CFLAGS += -fPIC
CFLAGS += -D_ZYNQ
CFLAGS += -std=c++11

all: $(LIBNAME)
#	cp -d *.so $(HOME)/targetfs-$(BOARD)/home/embedded/bardy
#	cp -d *.so.0 $(HOME)/targetfs-$(BOARD)/home/embedded/bardy
	cp -d *.so /tftpboot
#	cp -d *.so.0 /tftpboot/nfsroot-$(BOARD)/home/embedded/bardy

include $(BARDYDIR)/make/shared.mk
include $(BARDYDIR)/make/rules.mk
include $(BARDYDIR)/make/clean.mk
