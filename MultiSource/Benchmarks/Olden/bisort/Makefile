LEVEL = ../../../..

PROG     = bisort
CPPFLAGS = -DTORONTO
LDFLAGS  = -lm

ifdef LARGE_PROBLEM_SIZE
RUN_OPTIONS = 40000000
HASH_PROGRAM_OUTPUT = 1
else
RUN_OPTIONS = 700000
endif
include $(LEVEL)/MultiSource/Makefile.multisrc

