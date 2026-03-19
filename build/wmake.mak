!define DIR_SOURCE	../source
!define DIR_INCLUDE	../include
!define DIR_BUILD	.

!ifdef __NT__
!	define HOST win
!else ifdef %WINDIR
!	define HOST win
!else ifdef __UNIX__
!	define HOST psx
!else ifdef __LINUX__
!	define HOST psx
!else
!	error Host not defined!
!endif


!ifeq HOST win
!	define OBJ_EXT		obj
!	define RM			rm -f
!else ifeq HOST psx
!	define OBJ_EXT		o
!	define RM			rm -f
!else
!	error host not defined!
!endif



SRCS	= $(DIR_SOURCE)/main.c $(DIR_SOURCE)/files.c $(DIR_SOURCE)/options.c $(DIR_SOURCE)/log.c
OBJS	= $(DIR_BUILD)/main.$(OBJ_EXT) $(DIR_BUILD)/files.$(OBJ_EXT) $(DIR_BUILD)/options.$(OBJ_EXT) $(DIR_BUILD)/log.$(OBJ_EXT)

# valid TARGET_SYS's are: dos, w16, w32. default dos
!ifndef TARGET_SYS
	TARGET_SYS = dos
!endif

!ifeq TARGET_SYS dos
!	include "$(DIR_BUILD)/mif/dos.mif"
!else ifeq TARGET_SYS w16
!	include "$(DIR_BUILD)/mif/w16.mif"
!else ifeq TARGET_SYS w32
!	include "$(DIR_BUILD)/mif/w32.mif"
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
	$(RM) $(OBJS) $(NAME).$(EXT) $(NAME).sym *.log *.img
