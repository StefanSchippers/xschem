# --- configuration part --

# - generic configuration -
#  where scconfig source is; this is a path to a partial or full checkout of
#  svn://repo.hu/scconfig/trunk/src
SRC=src/

#  where compiled binaries (e.g. objects) should land; should be the same as
#  $(SRC) the project has its own copy of scconfig embedded
BIN=src/

#  what cflags to use to compile scconfig
#  If -DRUNTIME is specified, hooks.c should have the detect hook for runtime
USER_CFLAGS = -DGENCALL -DRUNTIME -g

#  what ldflags to use to link scconfig
USER_LDFLAGS =

#  in case hooks.c needs to link to something local
USER_OBJS =

#  what to build - a ./configure
all: configure

# This line imports scconfig core and default tests
include $(SRC)/default/Makefile.plugin

#
# - PLUGINS -
#
#  Comment this line if you are not interested in c99 features
#include $(SRC)/c99/Makefile.plugin

#  Comment this line if you do not need script libs to be detected
include $(SRC)/scripts/Makefile.plugin

#  Comment this line if you do not need parser libs to be detected
#include $(SRC)/parser/Makefile.plugin

#  Comment this line if you do not need to detect parser generators
include $(SRC)/parsgen/Makefile.plugin

#  Comment this line if you do not need math related libs
#include $(SRC)/math/Makefile.plugin

#  Comment this line if you do not need socket/networking
#include $(SRC)/socket/Makefile.plugin

#  Comment this line if you do not need user/password API detection
#include $(SRC)/userpass/Makefile.plugin

#  Comment this line if you do not need gui (X11, toolkits)
include $(SRC)/gui/Makefile.plugin

#  Comment this line if you do not need tty (nurses, slang, pty-related calls)
#include $(SRC)/tty/Makefile.plugin

#  Comment this line if you do not need software utility libs (gen*, glib)
#include $(SRC)/sul/Makefile.plugin

#  Comment this line if you do not need to detect POSIX calls
#include $(SRC)/posix/Makefile.plugin

#  Uncomment this line if you need menus
#include $(SRC)/menulib/Makefile.plugin

#  Comment this line if you do not need tmpasm (templating); conflicts with generator
include $(SRC)/tmpasm/Makefile.plugin

#  Uncomment this line if you need generator (deprecated templating); conflicts with tmpasm
#include $(SRC)/generator/Makefile.plugin

# --- you shouldn't edit the lines below ---
OBJS = $(USER_OBJS) hooks.o $(DEFAULT_NOMAIN_OBJS) $(SCRIPT_OBJS) $(PARSER_OBJS) $(GENERATOR_OBJS) $(TMPASM_OBJS) $(C99_OBJS) $(PARSGEN_OBJS) $(MATH_OBJS) $(SOCKET_OBJS) $(USERPASS_OBJS) $(GUI_OBJS) $(TTY_OBJS) $(SUL_OBJS) $(POSIX_OBJS)
CFLAGS = $(USER_CFLAGS) $(DEFAULT_CFLAGS) $(SCRIPT_CFLAGS) $(PARSER_CFLAGS) $(GENERATOR_CFLAGS) $(TMPASM_CFLAGS) $(C99_CFLAGS) $(PARSGEN_CFLAGS) $(MATH_CFLAGS) $(SOCKET_CFLAGS) $(USERPASS_CFLAGS) $(GUI_CFLAGS) $(TTY_CFLAGS) $(SUL_CFLAGS) $(POSIX_CFLAGS) $(MENULIB_CFLAGS) -I$(SRC)/default
LDFLAGS = $(USER_LDFLAGS) $(DEFAULT_LDFLAGS) $(SCRIPT_LDFLAGS) $(PARSER_LDFLAGS) $(GENERATOR_LDFLAGS) $(TMPASM_LDFLAGS) $(C99_LDFLAGS) $(PARSGEN_LDFLAGS) $(MATH_LDFLAGS) $(SOCKET_LDFLAGS) $(USERPASS_LDFLAGS) $(GUI_LDFLAGS) $(TTY_LDFLAGS) $(SUL_LDFLAGS) $(POSIX_LDFLAGS) $(MENULIB_LDFLAGS)

all: configure sccbox

configure: $(OBJS) $(DEFAULT_MAIN_OBJS)
	$(CC) -o configure $(OBJS) $(DEFAULT_MAIN_OBJS)

menuconfig: $(OBJS) $(MENULIB_OBJS)
	$(CC) -o configure $(OBJS) $(MENULIB_OBJS)

src/util/sccbox.o: src/util/sccbox.c
	$(CC) -c $(CFLAGS) -o src/util/sccbox.o src/util/sccbox.c

sccbox: src/util/sccbox.o
	$(CC) $(LDFLAGS) -o sccbox src/util/sccbox.o

clean:
	rm $(OBJS) $(DEFAULT_MAIN_OBJS) configure src/util/sccbox.o

distclean:
	$(MAKE) clean ; true
	rm -f config.cache config.log sccbox
