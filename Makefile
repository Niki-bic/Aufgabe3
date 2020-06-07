# @file Makefile
# BES sender/empfaenger
# Projekt 3
#
# authors:
#
# Binder Patrik             <ic19b030@technikum-wien.at>
# Ferchenbauer Nikolaus     <ic19b013@technikum-wien.at>
# Pittner Stefan            <ic19b003@technikum-wien.at>
# @date 2020/01/06
#
# @version 1.0.0
#
# --------------------------------------------------------------------------

# ------------------------------------------------------------- variables --
# tools and options:

CC     =  gcc
CFLAGS = -Wall -Werror -Wextra -Wstrict-prototypes -Wformat=2 -pedantic \
         -fno-common -ftrapv -O3 -g -c -std=gnu11 
TFLAGS = -lrt -lpthread
RM     =  rm -f

# filenames:

EXEC   = sender empfaenger
SRC    = sender.c empfaenger.c functions.c
OBJ    = $(SRC:.c=.o)
HEADER = sender_empfaenger.h
DOC    = doc
DOXY   = doxygen
DFILE  = Doxyfile

# --------------------------------------------------------------- targets --

# .PHONY

.PHONY: all clean doc distclean cleanall

all: $(EXEC)


clean:
	$(RM) $(EXEC) $(OBJ)

doc:
	$(DOXY) $(DFILE)

distclean:
	$(RM) -r $(DOC)

cleanall:
	$(RM) -r $(EXEC) $(OBJ) $(DOC)

# rules:

sender: sender.o functions.o
	$(CC) -o $@  $^  $(TFLAGS)

empfaenger: empfaenger.o functions.o
	$(CC) -o $@  $^  $(TFLAGS)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS)  $<

# --------------------------------------------------------------------------
