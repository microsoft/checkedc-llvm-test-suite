LEVEL = ../../../..

PROG     = office-ispell
CPPFLAGS = -Dconst=
CFLAGS  += -DNO_IMPLICIT_INCLUDE_CHECKED_HDRS
LDFLAGS  = -lm
RUN_OPTIONS = -a -d $(PROJ_SRC_DIR)/americanmed+
STDIN_FILENAME = $(PROJ_SRC_DIR)/large.txt

include $(LEVEL)/MultiSource/Makefile.multisrc
