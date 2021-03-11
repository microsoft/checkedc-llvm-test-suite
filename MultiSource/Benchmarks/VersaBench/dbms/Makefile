LEVEL = ../../../..

PROG     = dbms
CFLAGS  += -DNO_IMPLICIT_INCLUDE_CHECKED_HDRS
ifdef SMALL_PROBLEM_SIZE
RUN_OPTIONS = -i $(PROJ_SRC_DIR)/dbms.train.in
else
RUN_OPTIONS = -i $(PROJ_SRC_DIR)/dbms.ref.in
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

