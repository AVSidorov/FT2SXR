
LIBNAME  := libbase2fmc.so
SONAME   := $(LIBNAME).0

include $(BARDYDIR)/make/macros.mk

DIRS := $(GIPCYINC) \
$(BARDYDIR)/BRDINC \
$(BARDYDIR)/BRDINC/ctrladmpro \
$(BARDYDIR)/icrinc \
$(BARDYDIR)/BRDLIBS \
$(BARDYDIR)/BRDLIBS/BASESRV \
$(BARDYDIR)/BRDLIBS/DDSSRV \
$(BARDYDIR)/BRDLIBS/ADCSRV \
$(BARDYDIR)/BRDLIBS/ADDITION \
$(BARDYDIR)/SIDEDRV \
$(BARDYDIR)/SIDEDRV/base2fmc
INC := $(addprefix -I, $(DIRS))

EXTFILES := $(BARDYDIR)/SIDEDRV/sidedllini.cpp \
$(BARDYDIR)/SIDEDRV/sideentry.cpp \
$(BARDYDIR)/BRDLIBS/module.cpp \
$(BARDYDIR)/BRDLIBS/adm2if.cpp \
$(BARDYDIR)/BRDLIBS/service.cpp \
$(BARDYDIR)/BRDLIBS/useful.cpp \
$(BARDYDIR)/BRDLIBS/BASESRV/basefsrv.cpp \
$(BARDYDIR)/BRDLIBS/BASESRV/piosrv.cpp \
$(BARDYDIR)/BRDLIBS/DDSSRV/ddssrv.cpp \
$(BARDYDIR)/BRDLIBS/DDSSRV/ddsctrl.cpp \
$(BARDYDIR)/BRDLIBS/ADDITION/si571.cpp

SRC := $(wildcard *.cpp)
SRC += $(EXTFILES)
SOURCE += $(SRC)
OBJ_FILES := $(notdir $(SOURCE:.cpp=.o))

CFLAGS += $(INC)
CFLAGS += -fPIC
CFLAGS += -D_BASE2FMC

all: $(LIBNAME)
#	cp -d *.so /tftpboot/nfsroot-$(BOARD)/home/embedded/bardy
#	cp -d *.so.0 /tftpboot/nfsroot-$(BOARD)/home/embedded/bardy


include $(BARDYDIR)/make/shared.mk
include $(BARDYDIR)/make/rules.mk
include $(BARDYDIR)/make/clean.mk
