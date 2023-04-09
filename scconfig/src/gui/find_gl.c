/*
    scconfig - gui lib detection - opengl related calls
    Copyright (C) 2017  Tibor Palinkas
    Copyright (C) 2018  Aron Barath

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
		Contact (email and IRC): http://igor2.repo.hu/contact.html
*/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "libs.h"
#include "log.h"
#include "db.h"
#include "dep.h"
#include "find_gl.h"

int find_gl(const char *name, int logdepth, int fatal)
{
	char test_c[256];
	const char *test_c_templ =
		NL "#include <%s/gl.h>"
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	GLenum err = glGetError();"
		NL "	if (err == 0 || err == GL_INVALID_OPERATION)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/gl";
	char **id, *incdirs[] = {"GL", "OpenGL", NULL};
	char **cf, *cflgs[] = {"", "-I/usr/include/libdrm", NULL};
	const char **lf, *ldflgs[] = {"-lGL", "-lopengl32", "-framework OpenGL", NULL};
	char *cflags = NULL;
	char *ldflags = NULL;

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	report("Checking for gl... ");
	logprintf(logdepth, "find_gl: running pkg-config...\n");
	logdepth++;

	if (run_pkg_config(logdepth, "gl", &cflags, &ldflags) == 0) {
		for(id = incdirs; *id != NULL; id++) {
			sprintf(test_c, test_c_templ, *id);
			if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
				goto success;
		}
	}

	logdepth--;
	logprintf(logdepth, "find_gl: manual tries...\n");
	logdepth++;

	for(lf = ldflgs; *lf != NULL; lf++) {
		ldflags = strclone(*lf);
		for(cf = cflgs; *cf != NULL; cf++) {
			for(id = incdirs; *id != NULL; id++) {
				sprintf(test_c, test_c_templ, *id);
				if (try_icl(logdepth, node, test_c, NULL, *cf, ldflags) != 0)
					goto success;
			}
		}
		free(ldflags);
	}

	return try_fail(logdepth, node);

	success:;
	put("libs/gui/gl/include_prefix", *id);
	if (cflags != NULL)
		free(cflags);
	if (ldflags != NULL)
		free(ldflags);
	return 0;
}

int find_gl_vao(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	glBindVertexArray(0);"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/gl/vao";
	const char *cflags, *ldflags, *incs;

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	if (require("libs/gui/gl/*", logdepth, fatal))
		return try_fail(logdepth, node);

	cflags = get("libs/gui/gl/cflags");
	ldflags = get("libs/gui/gl/ldflags");
	incs = get("libs/gui/gl/includes");

	report("Checking for gl vao... ");
	logprintf(logdepth, "find_gl_vao...\n");
	logdepth++;

	if (try_icl_norun(logdepth, node, test_c, incs, cflags, ldflags) != 0)
		return 0;

	return try_fail(logdepth, node);
}

int find_gl_fb_attachment(const char *name, int logdepth, int fatal)
{
	const char *test_c_templ =
		NL "#include <stdio.h>"
		NL "#include <%s/gl.h>"
		NL "int main()"
		NL "{"
		NL "	int stencil_bits;"
		NL "	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_bits);"
		NL "	return 0;"
		NL "}"
		NL;
	char test_c[512];
	const char *node = "libs/gui/gl/fb_attachment";
	const char *cflags, *ldflags, *incs, *ip;

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	if (require("libs/gui/gl/*", logdepth, fatal))
		return try_fail(logdepth, node);

	cflags = get("libs/gui/gl/cflags");
	ldflags = get("libs/gui/gl/ldflags");
	incs = get("libs/gui/gl/includes");
	ip = get("libs/gui/gl/include_prefix");

	report("Checking for gl framebuffer attachment... ");
	logprintf(logdepth, "find_gl_fb_attachment...\n");
	logdepth++;

	sprintf(test_c, test_c_templ, ip);

	if (try_icl_norun(logdepth, node, test_c, incs, cflags, ldflags) != 0)
		return 0;

	return try_fail(logdepth, node);
}


int find_glu(const char *name, int logdepth, int fatal)
{
	char test_c[256];
	const char *test_c_templ =
		NL "#include <%s/gl.h>"
		NL "#include <%s/glu.h>"
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	GLUtesselator *g = gluNewTess();"
		NL "	if (g != NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/glu", *ipr;
	char **cf, *cflgs[] = {"", "-I/usr/include/libdrm", NULL};
	char *cflags = NULL;
	char *ldflags = NULL;

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);

	if (require("libs/gui/gl/include_prefix", logdepth, fatal))
		return try_fail(logdepth, node);

	ipr = get("libs/gui/gl/include_prefix");
	if (ipr == NULL)
		return try_fail(logdepth, node);

	sprintf(test_c, test_c_templ, ipr, ipr);

	report("Checking for glu... ");
	logprintf(logdepth, "find_glu: running pkg-config...\n");
	logdepth++;

	if (run_pkg_config(logdepth, "glu", &cflags, &ldflags) == 0) {
		if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
			goto success;
	}

	logdepth--;
	logprintf(logdepth, "find_glu: manual tries...\n");
	logdepth++;

	ldflags = strclone("-lGLU");
	for(cf = cflgs; *cf != NULL; cf++)
		if (try_icl(logdepth, node, test_c, NULL, *cf, ldflags) != 0)
			goto success;
	free(ldflags);

	ldflags = strclone("-lglu32");
	for(cf = cflgs; *cf != NULL; cf++)
		if (try_icl(logdepth, node, test_c, NULL, *cf, ldflags) != 0)
			goto success;
	free(ldflags);

	return try_fail(logdepth, node);

	success:;
	if (cflags != NULL)
		free(cflags);
	if (ldflags != NULL)
		free(ldflags);
	return 0;
}

int find_glut(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	glutGet(GLUT_ELAPSED_TIME);"
		NL "	if (glutGet!=NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/glut", *ipr;
	char freeglut[4096];
	char **inc, *incs[] = {"", "#include <openglut.h>", NULL};
	char *cflags = NULL;
	char *ldflags = NULL;

	incs[0] = freeglut;

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);


	report("Checking for glut... ");
	logprintf(logdepth, "find_glut: running pkg-config...\n");
	logdepth++;

	if (run_pkg_config(logdepth, "glut", &cflags, &ldflags) == 0) {
		if (try_icl(logdepth, node, test_c, NULL, cflags, ldflags) != 0)
			goto success;
	}

	if (require("libs/gui/gl/include_prefix", logdepth, fatal))
		return try_fail(logdepth, node);
	ipr = get("libs/gui/gl/include_prefix");
	if (ipr == NULL)
		return try_fail(logdepth, node);

	if (cflags != NULL) {
		free(cflags);
		cflags = NULL;
	}

	if (ldflags != NULL) {
		free(ldflags);
		ldflags = NULL;
	}

	logdepth--;
	logprintf(logdepth, "find_glut: manual tries...\n");
	logdepth++;

	sprintf(freeglut, "#include <%s/freeglut.h>", ipr);
	ldflags = strclone("-lglut");
	for(inc = incs; *inc != NULL; inc++)
		if (try_icl(logdepth, node, test_c, *inc, NULL, ldflags) != 0)
			goto success;
	free(ldflags);

	sprintf(freeglut, "#include <windows.h>\n#include <%s/glut.h>", ipr);
	ldflags = strclone("-lglut32");
	for(inc = incs; *inc != NULL; inc++)
		if (try_icl(logdepth, node, test_c, *inc, NULL, ldflags) != 0)
			goto success;
	free(ldflags);

	return try_fail(logdepth, node);

	success:;
	if (cflags != NULL)
		free(cflags);
	if (ldflags != NULL)
		free(ldflags);
	return 0;
}

int find_gui_wgl(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	if(wglGetCurrentDC() == NULL)"
		NL "		puts(\"OK\");"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/wgl";

	require("cc/cc", logdepth, fatal);

	report("Checking for wgl()... ");
	logprintf(logdepth, "find_gui_wgl: trying to find wgl...\n");
	logdepth++;

	/* wgl exists only on windows, so try what the documentation says */
	/* ("wgl" stands for "Windows GL" oslt) */
	if (try_icl(logdepth, node, test_c, "#include <wingdi.h>", NULL, "-lopengl32")) return 0;
	return try_fail(logdepth, node);
}

static int accept_glfw(char *stdout_str)
{
	int major = 0, minor = 0, rev = 0;
	char tmp[32];

	if ((stdout_str[0] != 'O') || (stdout_str[1] != 'K'))
		return 0;

	sscanf(stdout_str + 2, "%d %d %d", &major, &minor, &rev);
	sprintf(tmp, "%d", major); put("libs/gui/glfw/ver/major", tmp);
	sprintf(tmp, "%d", minor); put("libs/gui/glfw/ver/minor", tmp);
	sprintf(tmp, "%d", rev);   put("libs/gui/glfw/ver/rev", tmp);

	return 1;
}

int find_glfw(const char *name, int logdepth, int fatal)
{
	const char *test_c =
		NL "#include <stdio.h>"
		NL "int main()"
		NL "{"
		NL "	int major = 0, minor = 0, rev = 0;"
		NL "	glfwGetVersion(&major, &minor, &rev);"
		NL "	if (major > 0)"
		NL "		printf(\"OK %d %d %d\\n\", major, minor, rev);"
		NL "	return 0;"
		NL "}"
		NL;
	const char *node = "libs/gui/glfw", *ipr;
	char freeglfw[4096];
	char **inc, *incs[] = {"", "#include <GLFW/glfw3.h>", NULL};
	char *cflags = NULL;
	char *ldflags = NULL;

	incs[0] = freeglfw;
	freeglfw[0] = 0;

	if (require("cc/cc", logdepth, fatal))
		return try_fail(logdepth, node);


	report("Checking for glfw... ");
	logprintf(logdepth, "find_glfw: running pkg-config...\n");
	logdepth++;

	if (run_pkg_config(logdepth, "glfw3", &cflags, &ldflags) == 0) {
		if (try_icl_(logdepth, node, test_c, incs[1], cflags, ldflags, 1, accept_glfw) != 0)
			goto success;
	}

	if (require("libs/gui/gl/include_prefix", logdepth, fatal))
		return try_fail(logdepth, node);
	ipr = get("libs/gui/gl/include_prefix");
	if (ipr == NULL)
		return try_fail(logdepth, node);

	if (cflags != NULL) {
		free(cflags);
		cflags = NULL;
	}

	if (ldflags != NULL) {
		free(ldflags);
		ldflags = NULL;
	}

	logdepth--;
	logprintf(logdepth, "find_glfw: manual tries...\n");
	logdepth++;

	sprintf(freeglfw, "#include <%s/glfw.h>", ipr);
	ldflags = strclone("-lglfw");
	for(inc = incs; *inc != NULL; inc++)
		if (try_icl_(logdepth, node, test_c, *inc, NULL, ldflags, 1, accept_glfw) != 0)
			goto success;
	free(ldflags);

	return try_fail(logdepth, node);

	success:;
	if (cflags != NULL)
		free(cflags);
	if (ldflags != NULL)
		free(ldflags);
	return 0;
}
