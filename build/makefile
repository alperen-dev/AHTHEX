DIR_SOURCE	= ../source
DIR_INCLUDE	= ../include
DIR_BUILD	= .

# make it o if you compile under linux
OBJ_EXT	= obj

SRCS	= $(DIR_SOURCE)/ahthex.c $(DIR_SOURCE)/files.c $(DIR_SOURCE)/options.c $(DIR_SOURCE)/log.c
OBJS	= $(DIR_BUILD)/ahthex.$(OBJ_EXT) $(DIR_BUILD)/files.$(OBJ_EXT) $(DIR_BUILD)/options.$(OBJ_EXT) $(DIR_BUILD)/log.$(OBJ_EXT)

# valid TARGET_SYS's are: dos, win16, win32
!ifndef TARGET_SYS
	TARGET_SYS = dos
!endif

!ifeq TARGET_SYS dos
!	include "$(DIR_BUILD)/mif/dos.mif"
!else ifeq TARGET_SYS win16
!	include "$(DIR_BUILD)/mif/win16.mif"
!else ifeq TARGET_SYS win32
!	include "$(DIR_BUILD)/mif/win32.mif"
!else
!	error Platform not supported
!endif

!ifndef NAME
	NAME	= ahthex
!endif
!ifndef EXT
	EXT		= bin
!endif

all: $(NAME).$(EXT) .SYMBOLIC

$(NAME).$(EXT): $(OBJS)
#	echo $(LDFLAGS)
#	echo Command: $(CC) $(OBJS) -o $@ $(LDFLAGS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJS): $(SRCS)
#	echo $(CFLAGS)
#	echo Sources: $(SRCS)
#	echo Objects: $(OBJS)
#	echo Command: $(CC) $(CFLAGS) -c $<
    $(CC) $(CFLAGS) -c $<

clean: .SYMBOLIC
	rm -f $(OBJS) $(NAME).$(EXT) $(NAME).sym *.log
