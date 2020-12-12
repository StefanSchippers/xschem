# Compilation Instructions for xschem on MacOS
In order to compile xschem properly on MacOS, we always need to reference the X libraries from XQuartz
and change some compilation variables. The following dependencies are required:
- XQuartz: https://www.xquartz.org/releases/XQuartz-2.7.11.html
- Tcl: https://prdownloads.sourceforge.net/tcl/tcl8.6.10-src.tar.gz
- Tk: https://prdownloads.sourceforge.net/tcl/tk8.6.10-src.tar.gz
The first step is to install XQuartz and then compile Tcl and Tk. Nothing special on compiling these two, but
we need to specify the X libraries from XQuartz when compiling Tk. Let's use the path
/usr/local/opt/tcl-tk as destination folder.
## Tcl compilation
Extract the Tcl sources and then go to the unix folder:

cd <extracted-folder>/unix
./configure --prefix=/usr/local/opt/tcl-tk
make
make install
Tk compilati on
Same procedure as Tcl, but we need to specificy the Tcl and X libraries paths. XQuartz is installed on
/opt/X11 , so we do:
cd <extracted-folder>/unix
./configure --prefix=/usr/local/opt/tcl-tk --with-tcl=/usr/local/opt/tcl-
tk/lib --with-x --x-includes=/opt/X11/include --x-libraries=/opt/X11/lib
make
make install
xschem compilati on
Besides referencing the X libraries, we need to also point to the Tcl/Tk installation path. Let's use a recent
xschem repository and install it on ~/xschem-macos (adapt this to your username):
git clone https://github.com/StefanSchippers/xschem.git
cd xschem
./configure --prefix=/Users/$(whoami)/xschem-macos
1 / 2README.md
12/8/2020
Before running compile the application, we need to adjust Makefile.conf because the current configure
script doesn't support custom flags. So we need to replace the CFLAGS and LDFLAGS variables in that file
as below:
CFLAGS=-I/opt/X11/include -I/opt/X11/include/cairo -I/usr/local/opt/tcl-
tk/include -O2
LDFLAGS=-L/opt/X11/lib -L/usr/local/opt/tcl-tk/lib -lm -lcairo -lX11 -
lXrender -lxcb -lxcb-render -lX11-xcb -lXpm -ltcl8.6 -ltk8.6
Finally, we compile and install the application.
make
make install
The application will be placed in /Users/$(whoami)/xschem-macos/bin and can be started with
./xschem in that folder.
