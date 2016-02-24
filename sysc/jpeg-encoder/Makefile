
EXTRACFLAGS = -Wno-unknown-pragmas
EXTRA_LIBS =

MODULE = run
SRCS = $(wildcard *.cpp)

OBJS = obj/$(SRCS:.cpp=.o)

include $(FORSYDE_MAKEDEFS)

CFLAGS += -DFORSYDE_INTROSPECTION -DREAD_BITMAP -DWRITE_JPEG


