
EXTRACFLAGS = -Wno-unknown-pragmas
EXTRA_LIBS =

MODULE = run
SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

include ../Makefile.defs

CFLAGS += -DFORSYDE_INTROSPECTION -DREAD_BITMAP -DWRITE_JPEG


