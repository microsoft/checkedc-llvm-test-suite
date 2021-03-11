LEVEL = ../../../..

PROG     = pifft
CPPFLAGS = -DVERSION='"1.00"' -DCOMPDATE="\"today\"" -DCFLAGS='""' -DHOSTNAME="\"thishost\""
CFLAGS  += -DNO_IMPLICIT_INCLUDE_CHECKED_HDRS
LDFLAGS  = -lm
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = $(PROJ_SRC_DIR)/ref.in
else
RUN_OPTIONS = $(PROJ_SRC_DIR)/test.in
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

