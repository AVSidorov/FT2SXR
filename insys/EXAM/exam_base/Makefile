
PHONY = clean
TARGET_NAME = exam_base

include $(BARDYDIR)/make/macros.mk

DIRS := $(GIPCYINC) $(BARDYDIR)/BRDINC $(BARDYDIR)/BRDINC/ctrladmpro $(BARDYDIR)/icrinc \
$(BARDYDIR)/BRDLIBS
INC := $(addprefix  -I, $(DIRS))

CFLAGS += $(INC)

SRC := $(wildcard *.cpp)
SOURCE += $(SRC)
OBJ_FILES := $(SOURCE:.cpp=.o)

all: $(TARGET_NAME)
#	cp $(TARGET_NAME) $(BARDYDIR)/bin
#	cp $(TARGET_NAME) /tftpboot/nfsroot-$(BOARD)/home/embedded/examples


include $(BARDYDIR)/make/rules.mk
include $(BARDYDIR)/make/clean.mk
include $(BARDYDIR)/make/app.mk