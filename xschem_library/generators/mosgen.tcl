#!/bin/sh
# the next line restarts using wish \
exec tclsh "$0" "$@"

set model [lindex $argv 0]

if {$model eq {nfet_g5v0d10v5}} {
puts {

v {xschem version=3.1.0 file_version=1.2}
G {}
K {type=nmos
lvs_format="@spiceprefix@name @pinlist sky130_fd_pr__@model L=@L W=@W nf=@nf m=@mult"
format="@spiceprefix@name @pinlist sky130_fd_pr__@model L=@L W=@W
+ nf=@nf ad=@ad as=@as pd=@pd ps=@ps
+ nrd=@nrd nrs=@nrs sa=@sa sb=@sb sd=@sd 
+ mult=@mult m=@mult"
template="name=M1
L=0.5
W=1
nf=1
mult=1
ad=\\"'int((nf+1)/2) * W/nf * 0.29'\\" 
pd=\\"'2*int((nf+1)/2) * (W/nf + 0.29)'\\"
as=\\"'int((nf+2)/2) * W/nf * 0.29'\\" 
ps=\\"'2*int((nf+2)/2) * (W/nf + 0.29)'\\"
nrd=\\"'0.29 / W'\\" nrs=\\"'0.29 / W'\\"
sa=0 sb=0 sd=0
model=nfet_g5v0d10v5
spiceprefix=X
"}
V {}
S {}
E {}
L 4 20 -30 20 -17.5 {}
L 4 20 17.5 20 30 {}
L 4 7.5 -17.5 20 -17.5 {}
L 4 7.5 17.5 15 17.5 {}
L 4 7.5 -22.5 7.5 22.5 {}
L 4 -20 0 -2.5 0 {}
B 5 17.5 -32.5 22.5 -27.5 {name=D dir=inout}
B 5 -22.5 -2.5 -17.5 2.5 {name=G dir=in}
B 5 17.5 27.5 22.5 32.5 {name=S dir=inout}
B 5 19.921875 -0.078125 20.078125 0.078125 {name=B dir=in}
P 4 4 15 15 20 17.5 15 20 15 15 {fill=true}
P 4 5 -2.5 15 -2.5 -15 2.5 -15 2.5 15 -2.5 15 {}
P 5 4 20 -2.5 15 0 20 2.5 20 -2.5 {fill=true}
T {@name} 5 -30 0 1 0.2 0.2 {}
T {S} 22.5 17.5 0 0 0.15 0.15 {layer=7}
T {D} 22.5 -17.5 2 1 0.15 0.15 {layer=7}
T {B} 20 -10 0 0 0.15 0.15 {layer=7}
T {G} -10 -10 0 1 0.15 0.15 {layer=7}
T {@model} 30 -8.75 2 1 0.2 0.2 {}
T {@mult x @W / @L} 31.25 13.75 0 0 0.2 0.2 { layer=13}
T {nf=@nf} 31.25 1.25 0 0 0.2 0.2 { layer=13}
T {tcleval(gm=[to_eng [ngspice::get_node [subst -nocommand \{\\@m.$\{path\}@spiceprefix@name\\.msky130_fd_pr__@model\\[gm]\}]]] )} 32.5 -8.75 0 0 0.15 0.15 {layer=15
hide=true}
T {tcleval(id=[to_eng [ngspice::get_node [subst -nocommand \{i(\\@m.$\{path\}@spiceprefix@name\\.msky130_fd_pr__@model\\[id])\}]]] )} 32.5 -30 0 0 0.15 0.15 {layer=15
hide=true}
}
} elseif {$model eq {pfet_g5v0d10v5}} {
puts {v {xschem version=3.1.0 file_version=1.2}
G {}
K {type=pmos
lvs_format="@spiceprefix@name @pinlist sky130_fd_pr__@model L=@L W=@W nf=@nf m=@mult"
format="@spiceprefix@name @pinlist sky130_fd_pr__@model L=@L W=@W
+ nf=@nf ad=@ad as=@as pd=@pd ps=@ps
+ nrd=@nrd nrs=@nrs sa=@sa sb=@sb sd=@sd 
+ mult=@mult m=@mult"
template="name=M1
L=0.5
W=1
nf=1
mult=1
ad=\\"'int((nf+1)/2) * W/nf * 0.29'\\" 
pd=\\"'2*int((nf+1)/2) * (W/nf + 0.29)'\\"
as=\\"'int((nf+2)/2) * W/nf * 0.29'\\" 
ps=\\"'2*int((nf+2)/2) * (W/nf + 0.29)'\\"
nrd=\\"'0.29 / W'\\" nrs=\\"'0.29 / W'\\"
sa=0 sb=0 sd=0
model=pfet_g5v0d10v5
spiceprefix=X
"}
V {}
S {}
E {}
L 4 20 -30 20 -17.5 {}
L 4 20 17.5 20 30 {}
L 4 7.5 17.5 20 17.5 {}
L 4 12.5 -17.5 20 -17.5 {}
L 4 -20 0 -12.5 0 {}
L 4 7.5 -22.5 7.5 22.5 {}
B 5 17.5 27.5 22.5 32.5 {name=D dir=inout}
B 5 -22.5 -2.5 -17.5 2.5 {name=G dir=in}
B 5 17.5 -32.5 22.5 -27.5 {name=S dir=inout}
B 5 19.921875 -0.078125 20.078125 0.078125 {name=B dir=in}
A 4 -7.5 0 5 180 360 {}
P 4 4 12.5 -20 7.5 -17.5 12.5 -15 12.5 -20 {fill=true}
P 4 5 -2.5 15 -2.5 -15 2.5 -15 2.5 15 -2.5 15 {}
P 5 4 15 -2.5 20 0 15 2.5 15 -2.5 {fill=true}
T {@name} 5 -30 0 1 0.2 0.2 {}
T {D} 22.5 17.5 0 0 0.15 0.15 {layer=7}
T {S} 22.5 -17.5 2 1 0.15 0.15 {layer=7}
T {B} 20 -10 0 0 0.15 0.15 {layer=7}
T {G} -11.875 -10 0 1 0.15 0.15 {layer=7}
T {@model} 30 -8.75 2 1 0.2 0.2 {}
T {@mult x @W / @L} 31.25 13.75 0 0 0.2 0.2 { layer=13}
T {nf=@nf} 31.25 1.25 0 0 0.2 0.2 { layer=13}
T {tcleval(gm=[to_eng [ngspice::get_node [subst -nocommand \{\\@m.$\{path\}@spiceprefix@name\\.msky130_fd_pr__@model\\[gm]\}]]] )} 32.5 -8.75 0 0 0.15 0.15 {layer=15
hide=true}
T {tcleval(id=[to_eng [ngspice::get_node [subst -nocommand \{i(\\@m.$\{path\}@spiceprefix@name\\.msky130_fd_pr__@model\\[id])\}]]] )} 32.5 -30 0 0 0.15 0.15 {layer=15
hide=true}
}
} elseif {$model eq {pfet_01v8}} {
puts {v {xschem version=3.1.0 file_version=1.2}
G {}
K {type=pmos
lvs_format="@spiceprefix@name @pinlist sky130_fd_pr__@model L=@L W=@W nf=@nf m=@mult"
format="@spiceprefix@name @pinlist sky130_fd_pr__@model L=@L W=@W
+ nf=@nf ad=@ad as=@as pd=@pd ps=@ps
+ nrd=@nrd nrs=@nrs sa=@sa sb=@sb sd=@sd 
+ mult=@mult m=@mult"
template="name=M1
L=0.15
W=1
nf=1
mult=1
ad=\\"'int((nf+1)/2) * W/nf * 0.29'\\" 
pd=\\"'2*int((nf+1)/2) * (W/nf + 0.29)'\\"
as=\\"'int((nf+2)/2) * W/nf * 0.29'\\" 
ps=\\"'2*int((nf+2)/2) * (W/nf + 0.29)'\\"
nrd=\\"'0.29 / W'\\" nrs=\\"'0.29 / W'\\"
sa=0 sb=0 sd=0
model=pfet_01v8
spiceprefix=X
"}
V {}
S {}
E {}
L 4 7.5 -22.5 7.5 22.5 {}
L 4 20 -30 20 -17.5 {}
L 4 20 17.5 20 30 {}
L 4 2.5 -15 2.5 15 {}
L 4 7.5 17.5 20 17.5 {}
L 4 12.5 -17.5 20 -17.5 {}
L 4 -20 0 -7.5 0 {}
B 5 17.5 27.5 22.5 32.5 {name=D dir=inout}
B 5 -22.5 -2.5 -17.5 2.5 {name=G dir=in}
B 5 17.5 -32.5 22.5 -27.5 {name=S dir=inout}
B 5 19.921875 -0.078125 20.078125 0.078125 {name=B dir=in}
A 4 -2.5 0 5 180 360 {}
P 4 4 12.5 -20 7.5 -17.5 12.5 -15 12.5 -20 {fill=true}
P 5 4 15 -2.5 20 0 15 2.5 15 -2.5 {fill=true}
T {@name} 5 -30 0 1 0.2 0.2 {}
T {D} 22.5 17.5 0 0 0.15 0.15 {layer=7}
T {S} 22.5 -17.5 2 1 0.15 0.15 {layer=7}
T {B} 20 -10 0 0 0.15 0.15 {layer=7}
T {G} -10 -10 0 1 0.15 0.15 {layer=7}
T {@model} 30 -8.75 2 1 0.2 0.2 {}
T {@mult x @W / @L} 31.25 13.75 0 0 0.2 0.2 { layer=13}
T {nf=@nf} 31.25 1.25 0 0 0.2 0.2 { layer=13}
T {tcleval(gm=[to_eng [ngspice::get_node [subst -nocommand \{\\@m.$\{path\}@spiceprefix@name\\.msky130_fd_pr__@model\\[gm]\}]]] )} 32.5 -8.75 0 0 0.15 0.15 {layer=15
hide=true}
T {tcleval(id=[to_eng [ngspice::get_node [subst -nocommand \{i(\\@m.$\{path\}@spiceprefix@name\\.msky130_fd_pr__@model\\[id])\}]]] )} 32.5 -30 0 0 0.15 0.15 {layer=15
hide=true}
}


} else { ;# default: nfet_01v8
puts {v {xschem version=3.1.0 file_version=1.2 }
G {}
K {type=nmos
lvs_format="@spiceprefix@name @pinlist sky130_fd_pr__@model L=@L W=@W nf=@nf m=@mult"
format="@spiceprefix@name @pinlist sky130_fd_pr__@model L=@L W=@W
+ nf=@nf ad=@ad as=@as pd=@pd ps=@ps
+ nrd=@nrd nrs=@nrs sa=@sa sb=@sb sd=@sd 
+ mult=@mult m=@mult"
template="name=M1
L=0.15
W=1
nf=1 
mult=1
ad=\\"'int((nf+1)/2) * W/nf * 0.29'\\" 
pd=\\"'2*int((nf+1)/2) * (W/nf + 0.29)'\\"
as=\\"'int((nf+2)/2) * W/nf * 0.29'\\" 
ps=\\"'2*int((nf+2)/2) * (W/nf + 0.29)'\\"
nrd=\\"'0.29 / W'\\" nrs=\\"'0.29 / W'\\"
sa=0 sb=0 sd=0
model=nfet_01v8
spiceprefix=X
"}
V {}
S {}
E {}
L 4 7.5 -22.5 7.5 22.5 {}
L 4 -20 0 2.5 0 {}
L 4 20 -30 20 -17.5 {}
L 4 20 17.5 20 30 {}
L 4 2.5 -15 2.5 15 {}
L 4 7.5 -17.5 20 -17.5 {}
L 4 7.5 17.5 15 17.5 {}
B 5 17.5 -32.5 22.5 -27.5 {name=D dir=inout}
B 5 -22.5 -2.5 -17.5 2.5 {name=G dir=in}
B 5 17.5 27.5 22.5 32.5 {name=S dir=inout}
B 5 19.921875 -0.078125 20.078125 0.078125 {name=B dir=in}
P 4 4 15 15 20 17.5 15 20 15 15 {fill=true}
P 5 4 20 -2.5 15 0 20 2.5 20 -2.5 {fill=true}
T {@name} 5 -30 0 1 0.2 0.2 {}
T {S} 22.5 17.5 0 0 0.15 0.15 {layer=7}
T {D} 22.5 -17.5 2 1 0.15 0.15 {layer=7}
T {B} 20 -10 0 0 0.15 0.15 {layer=7}
T {G} -10 -10 0 1 0.15 0.15 {layer=7}
T {@model} 30 -8.75 2 1 0.2 0.2 {}
T {@mult x @W / @L} 31.25 13.75 0 0 0.2 0.2 { layer=13}
T {nf=@nf} 31.25 1.25 0 0 0.2 0.2 { layer=13}
T {tcleval(gm=[to_eng [ngspice::get_node [subst -nocommand \{\\@m.$\{path\}@spiceprefix@name\\.msky130_fd_pr__@model\\[gm]\}]]] )} 32.5 -8.75 0 0 0.15 0.15 {layer=15
hide=true}
T {tcleval(id=[to_eng [ngspice::get_node [subst -nocommand \{i(\\@m.$\{path\}@spiceprefix@name\\.msky130_fd_pr__@model\\[id])\}]]] )} 32.5 -30 0 0 0.15 0.15 {layer=15
hide=true}
}
}
