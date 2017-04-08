LEVEL = ../../../..
PROG = ks
#OBJS = KS-1.o KS-2.o
ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS += $(PROJ_SRC_DIR)/KL-5.in
HASH_PROGRAM_OUTPUT = 1
else
RUN_OPTIONS += $(PROJ_SRC_DIR)/KL-4.in
endif

include $(LEVEL)/MultiSource/Makefile.multisrc

