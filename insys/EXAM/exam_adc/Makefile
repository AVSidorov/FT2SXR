
PHONY = clean
TARGET_NAME = exam_adc

include $(BARDYDIR)/make/macros.mk

DIRS := $(GIPCYINC) $(BARDYDIR)/BRDINC $(BARDYDIR)/BRDINC/ctrladmpro $(BARDYDIR)/icrinc \
$(BARDYDIR)/BRDLIBS
INC := $(addprefix  -I, $(DIRS))

CFLAGS += $(INC)

SRC := $(wildcard *.cpp)
SOURCE += $(SRC) $(BARDYDIR)/BRDLIBS/reg_rw_spd.cpp
OBJ_FILES := $(SOURCE:.cpp=.o)

all: $(TARGET_NAME)
	cp $(TARGET_NAME) $(BARDYDIR)/bin

include $(BARDYDIR)/make/rules.mk
include $(BARDYDIR)/make/clean.mk
include $(BARDYDIR)/make/app.mk