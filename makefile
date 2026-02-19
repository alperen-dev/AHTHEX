SRCS = ahthex.c files.c lnklst.c options.c log.c

# valid TARGET_SYS's are: dos, win16, win32
!ifndef TARGET_SYS
	TARGET_SYS = dos
!endif

!ifeq TARGET_SYS dos
!	include "mif/dos.mif"
!else ifeq TARGET_SYS win16
!	include "mif/win16.mif"
!else ifeq TARGET_SYS win32
!	include "mif/win32.mif"
!else
!	error Platform not supported
!endif

OBJS	= $(SRCS:.c=.o)

!ifndef NAME
	NAME	= ahthex
!endif
!ifndef EXT
	EXT		= out
!endif

all: $(NAME).$(EXT) .SYMBOLIC

$(NAME).$(EXT): $(OBJS)
	echo $(LDFLAGS)
	echo Command: $(CC) $(OBJS) -o $@ $(LDFLAGS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJS): $(SRCS)
#	echo $(CFLAGS)
#	echo Sources: $(SRCS)
#	echo Objects: $(OBJS)
#	echo Command: $(CC) $(CFLAGS) -c $<
    $(CC) $(CFLAGS) -c $<

clean: .SYMBOLIC
	rm -f $(OBJS) $(NAME).$(EXT) $(NAME).sym
