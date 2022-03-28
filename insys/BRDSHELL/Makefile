
LIBNAME  := libbrd.so
SONAME   := $(LIBNAME).0

include $(BARDYDIR)/make/macros.mk

DIRS := $(GIPCYINC) $(BARDYDIR)/BRDINC $(BARDYDIR)/BRDLIBS
INC := $(addprefix -I, $(DIRS))

SRC := $(wildcard *.cpp)
SOURCE += $(SRC)
OBJ_FILES := $(notdir $(SOURCE:.cpp=.o))

CFLAGS += $(INC)
CFLAGS += -fPIC

all: $(LIBNAME)
#	cp -d *.so /tftpboot/nfsroot-$(BOARD)/home/embedded/bardy
#	cp -d *.so.0 /tftpboot/nfsroot-$(BOARD)/home/embedded/bardy
	cp -d *.so /tftpboot


include $(BARDYDIR)/make/shared.mk
include $(BARDYDIR)/make/rules.mk
include $(BARDYDIR)/make/clean.mk
