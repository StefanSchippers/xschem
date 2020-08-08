#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"

#include "find_x.h"
#include "find_gtk2.h"
#include "find_gtk3.h"
#include "find_lesstif2.h"
#include "find_gd.h"
#include "find_cairo.h"
#include "find_misc.h"
#include "find_gl.h"

void deps_gui_init()
{
	dep_add("libs/gui/xopendisplay/*",      find_xopendisplay);
	dep_add("libs/gui/xinerama/*",          find_xinerama);
	dep_add("libs/gui/xrender/*",           find_xrender);
	dep_add("libs/gui/xcb/*",               find_xcb);
	dep_add("libs/gui/xcb_render/*",        find_xcb_render);
	dep_add("libs/gui/xgetxcbconnection/*", find_xgetxcbconnection);
	dep_add("libs/gui/xpm/*",               find_xpm);
	dep_add("libs/gui/gtk2/*",              find_gtk2);
	dep_add("libs/gui/gtk2gl/*",            find_gtk2gl);
	dep_add("libs/gui/gtk2/key_prefix",     find_gtk2_key_prefix);
	dep_add("libs/gui/gtk2/modversion",     find_gtk2_modversion);
	dep_add("libs/gui/gtk3/*",              find_gtk3);
	dep_add("libs/gui/lesstif2/*",          find_lesstif2);
	dep_add("libs/gui/libstroke/*",         find_libstroke);
	dep_add("libs/gui/gd/gdImagePng/*",     find_gdimagepng);
	dep_add("libs/gui/gd/gdImageGif/*",     find_gdimagegif);
	dep_add("libs/gui/gd/gdImageJpeg/*",    find_gdimagejpeg);
	dep_add("libs/gui/gd/gdImageSetResolution/*", find_gdimagesetresolution);
	dep_add("libs/gui/gd/*",                find_gd);
	dep_add("libs/gui/cairo/*",             find_cairo);
	dep_add("libs/gui/cairo-xcb/*",         find_cairo_xcb);
	dep_add("libs/gui/gl/*",                find_gl);
	dep_add("libs/gui/glu/*",               find_glu);
	dep_add("libs/gui/glut/*",              find_glut);
	dep_add("libs/gui/wgl/*",               find_gui_wgl);
}
