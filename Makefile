#DEBUG = YES
APP_NAME = test
TEC_UNAME=Linux26g4

CC = icc

SRCINCLUDE = data_structure.c perf_test.c cohesive.c poly.c cohesive3D.c
SRCINCLUDE := $(addprefix include/, $(SRCINCLUDE))
SRCS =  main.c $(SRCINCLUDE)


OBJS := $(SRCS)
#OBJS := $(OBJS:.c=.o)
#OBJS := $(OBJS:.C=.o)
#OBJS := $(OBJS:.cpp=.o)

INCLUDES = -I/include.

LIBS = -lm

$(APP_NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< $(DEFINES) $(INCLUDES)

#%.o: %.C
#	$(CC) $(CFLAGS) -o $@ -c $< $(DEFINES) $(INCLUDES)

#%.o: %.cpp
#	$(CC) $(CFLAGS) -o $@ -c $< $(DEFINES) $(INCLUDES)

.PHONY : clean
clean:
	-rm -f $(APP_NAME)

#.PHONY : re
#re: clean $(APP_NAME)
