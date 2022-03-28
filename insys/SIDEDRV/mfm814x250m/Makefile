
LIBNAME  := libmfm814x250m.so
SONAME   := $(LIBNAME).0

include $(BARDYDIR)/make/macros.mk

DIRS := $(GIPCYINC) \
$(BARDYDIR)/BRDINC \
$(BARDYDIR)/BRDINC/ctrladmpro \
$(BARDYDIR)/icrinc \
$(BARDYDIR)/BRDLIBS \
$(BARDYDIR)/BRDLIBS/BASESRV \
$(BARDYDIR)/BRDLIBS/ADCSRV \
$(BARDYDIR)/BRDLIBS/ADDITION \
$(BARDYDIR)/SIDEDRV \
$(BARDYDIR)/SIDEDRV/mfm814x250m
INC := $(addprefix -I, $(DIRS))

EXTFILES := $(BARDYDIR)/SIDEDRV/sidedllini.cpp \
$(BARDYDIR)/SIDEDRV/sideentry.cpp \
$(BARDYDIR)/BRDLIBS/submodule.cpp \
$(BARDYDIR)/BRDLIBS/module.cpp \
$(BARDYDIR)/BRDLIBS/adm2if.cpp \
$(BARDYDIR)/BRDLIBS/adcsubmodule.cpp \
$(BARDYDIR)/BRDLIBS/service.cpp \
$(BARDYDIR)/BRDLIBS/ADCSRV/adcctrl.cpp \
$(BARDYDIR)/BRDLIBS/ADCSRV/adcsrv.cpp \
$(BARDYDIR)/BRDLIBS/ADDITION/si571.cpp \
$(BARDYDIR)/BRDLIBS/useful.cpp

SRC := $(wildcard *.cpp)
SRC += $(EXTFILES)
SOURCE += $(SRC)
OBJ_FILES := $(notdir $(SOURCE:.cpp=.o))

CFLAGS += $(INC)
CFLAGS += -fPIC
CFLAGS += -D_MFM814x250M

all: $(LIBNAME)

include $(BARDYDIR)/make/shared.mk
include $(BARDYDIR)/make/rules.mk
include $(BARDYDIR)/make/clean.mk
