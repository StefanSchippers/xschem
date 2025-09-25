v {xschem version=3.4.8RC file_version=1.3
*
* Analyses library for visual circuit analysis setup. 
* Copyright (C) 2025 Arpad Buermen
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
}
G {}
K {}
V {}
S {}
F {}
E {}
C {op.sym} 470 1220 0 0 {name=op1
only_toplevel=false 
order="1"
sweep="inner"
nodeset=""
store=""
write=""
}
C {sweep.sym} 290 1220 0 0 {name=inner
only_toplevel=false
tag="vds"
sweep="outer"
instance="\\"vds\\""
model=""
parameter="\\"dc\\""
option=""
variable=""
from=0
to=1.2
step=0.01
mode=""
points=""
values=""
}
C {sweep.sym} 110 1220 0 0 {name=outer
only_toplevel=false
tag="vgs"
sweep=""
instance="\\"vgs\\""
model=""
parameter="\\"dc\\""
option=""
variable=""
from=0
to=1.2
step=0.2
mode=""
points=""
values=""
}
C {tran.sym} 470 1360 0 0 {name=tran1
only_toplevel=false 
order="2"
sweep=""
step="1u"
stop="100u"
start=""
maxstep=""
icmode=""
nodeset=""
ic=""
store=""
write=""
}
C {verbatim.sym} 470 1520 0 0 {name=verbatim1
only_toplevel=false 
order="3"
simulator="vacask"
verbatim="// A comment
// Another one"
}
C {dc1d.sym} 720 1110 0 0 {name=dc1
only_toplevel=false 
order="4"
tag="sweep"
sweep=""
instance="\\"v1\\""
model=""
parameter=""
option=""
variable=""
from=0
to=0.1
step=0.01
mode=""
points=""
values=""
continuation=""
nodeset=""
store=""
write=""
}
C {dcinc.sym} 720 1320 0 0 {name=dcinc1
only_toplevel=false 
order="5"
sweep=""
nodeset=""
store=""
write=""
writeop="1"
}
C {dcxf.sym} 720 1460 0 0 {name=dcxf1
only_toplevel=false 
order=""
sweep=""
outp="\\"out\\""
outn=""
in="\\"v1\\""
nodeset=""
store=""
write=""
writeop=""
}
C {ac.sym} 720 1600 0 0 {name=ac1
only_toplevel=false 
order="7"
sweep=""
from=10
to=100k
step=""
mode="\\"dec\\""
points="10"
values=""
nodeset="[\\"a\\", 0.5]"
store=""
write=""
writeop=""
}
C {acxf.sym} 960 1100 0 0 {name=acxf1
only_toplevel=false 
order="8"
sweep=""
outp="\\"out\\""
outn=""
from=10
to=100k
step=""
mode="\\"dec\\""
points="10"
values=""
nodeset=""
store=""
write=""
writeop=""
}
C {noise.sym} 960 1340 0 0 {name=noise1
only_toplevel=false 
order="9"
sweep=""
outp="\\"out\\""
outn=""
in=\\"vin\\"
from=10
to=100k
step=""
mode="\\"dec\\""
points="10"
values=""
ptssum="1"
nodeset=""
store=""
write=""
writeop=""
}
C {hb.sym} 960 1590 0 0 {name=hb1
only_toplevel=false 
order="10"
sweep=""
freq=[100k]
nharm=10
immax=""
truncate="\\"box\\""
samplefac=""
nper=""
sample=""
harmonic=""
imorder=""
nodeset=""
store=""
write=""
}
C {postproc.sym} 1160 1100 0 0 {name=postproc1
only_toplevel=false 
order="12"
tool="PYTHON"
file="\\"script.py\\""
}
C {verbatim.sym} 470 1680 0 0 {name=verbatim2
only_toplevel=false 
order="3"
simulator="ngspice"
verbatim="* A comment
* Another one"
}
C {command_block.sym} 470 1110 0 0 {name=CMD
only_toplevel=false
}
