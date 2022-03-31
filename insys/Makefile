
TOP := $(dir $(lastword $(MAKEFILE_LIST)))

JLEVEL := 4
EARLY_DIRS := gipcy BRDSHELL

BARDYDIR ?= $(shell pwd)/../
BARDYLIB ?= $(BARDYDIR)/bin
GIPCYDIR ?= $(BARDYDIR)/gipcy
GIPCYLIB ?= $(GIPCYDIR)/lib
GIPCYINC ?= $(GIPCYDIR)/include

include ./make/checkenv.mk
include ./make/recurse.mk
