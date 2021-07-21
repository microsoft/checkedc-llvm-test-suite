LEVEL = ../../../..

PROG       = treeadd
CPPFLAGS   = -DTORONTO
LDFLAGS    = 

ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = 20
else ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 29
else
RUN_OPTIONS = 22
ifeq ($(ARCH),XCore)
XCORE_TARGET_NEEDS_MEMORY := 128
endif
endif

include $(LEVEL)/MultiSource/Makefile.multisrc

