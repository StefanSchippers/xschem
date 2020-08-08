/*
    scconfig - glue layer for proper crosscompiling to target
    Copyright (C) 2009  Tibor Palinkas

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

		Project page: http://repo.hu/projects/scconfig
		Contact via email: scconfig [at] igor2.repo.hu
*/

#include <stdlib.h>
#include "db.h"
#include "libs.h"

int find_target(const char *name, int logdepth, int fatal)
{
	const char *target = get("/arg/sys/target");
	const char *emu = get("/arg/sys/emu");

	(void) logdepth; /* to suppress compiler warnings about not using logdepth */
	(void) fatal;    /* to suppress compiler warnings about not using fatal */

	/* Does target differ from host? */
	if (target == NULL) {
		db_link("/host", "/target");
#ifdef RUNTIME
		db_link("/host", "/runtime");
#endif
		put("/target/sys/cross", sfalse);
		put("/target/sys/cross_blind", sfalse);
		return 0;
	}
	else
		db_mkdir("/target");

	put("/target/sys/target", target);
	put("/target/sys/cross", strue);
	if (emu != NULL)
		put("/target/sys/emu", emu);

	/* If so, check if emulator is provided */
	cross_blind = ((emu == NULL) || (*emu == '\0'));
	put("/target/sys/cross_blind", cross_blind ? strue : sfalse);

	return 0;
}
