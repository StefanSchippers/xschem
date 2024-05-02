#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

#include "find_ncurses.h"
#include "find_slang.h"
#include "find_gpm.h"
#include "find_pty.h"
#include "find_ioctl.h"
#include "find_term.h"
#include "find_readline.h"

void deps_tty_init()
{
	dep_add("libs/tty/slang/*",            find_slang);

	dep_add("libs/tty/gpm/*",              find_gpm);

	dep_add("libs/tty/grantpt/*",          find_grantpt);
	dep_add("libs/tty/posix_openpt/*",     find_posix_openpt);

	dep_add("libs/tty/ncurses/escdelay/*", find_ncurses_escdelay);
	dep_add("libs/tty/ncurses/cur_term/*", find_ncurses_cur_term);
	dep_add("libs/tty/ncurses/resizeterm/*",find_ncurses_resizeterm);
	dep_add("libs/tty/ncurses/*",          find_ncurses);
	dep_add("libs/tty/ncursesw/*",         find_ncursesw);

	dep_add("libs/tty/ioctl/*",            find_tty_ioctl);

	dep_add("libs/tty/cfmakeraw/*",        find_tty_cfmakeraw);
	dep_add("libs/tty/cfsetspeed/*",       find_tty_cfsetspeed);

	dep_add("libs/tty/readline/*",         find_tty_readline);
	dep_add("libs/tty/history/*",          find_tty_history);
}
