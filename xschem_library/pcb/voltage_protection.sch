v {xschem version=3.4.4 file_version=1.2
*
* This file is part of XSCHEM,
* a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
* simulation.
* Copyright (C) 1998-2024 Stefan Frederik Schippers
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
K {type=subcircuit
format="@name @pinlist @symname"
template="name=x1"}
V {}
S {}
E {}
B 2 170 -840 1010 -100 {dash=4}
T {@name} 1010 -870 0 1 0.4 0.4 {}
T {@symname} 170 -870 0 0 0.4 0.4 {}
N 920 -520 920 -390 {lab=VCC_FIVE}
N 750 -360 880 -360 {lab=G}
N 920 -300 1010 -300 {lab=VOUT}
N 920 -170 920 -150 {lab=VSS}
N 920 -300 920 -230 {lab=VOUT}
N 480 -310 480 -220 {lab=Z}
N 480 -390 710 -390 {lab=B}
N 480 -160 480 -130 {lab=VSSA}
N 480 -430 480 -390 {lab=B}
N 750 -520 750 -420 {lab=VCC_FIVE}
N 750 -260 750 -230 {lab=#net1}
N 750 -170 750 -150 {lab=VSS}
N 480 -390 480 -370 {lab=B}
N 920 -330 920 -300 {lab=VOUT}
N 750 -360 750 -320 {lab=G}
N 400 -160 400 -130 {lab=VSSA}
N 400 -220 480 -220 {lab=Z}
N 700 -320 800 -320 {lab=G}
N 700 -260 800 -260 {lab=#net1}
N 480 -520 480 -490 { lab=VCC_FIVE}
N 480 -520 920 -520 { lab=VCC_FIVE}
N 920 -710 920 -520 { lab=VCC_FIVE}
N 170 -710 310 -710 { lab=VCC_UNREG}
N 170 -630 310 -630 { lab=VSSA}
N 820 -710 920 -710 { lab=VCC_FIVE}
N 260 -740 260 -710 { lab=VCC_UNREG}
C {conn_3x1.sym} 300 -360 0 0 {name=C1}
C {opin.sym} 1010 -300 0 0 {name=p0 lab=VOUT}
C {lab_wire.sym} 810 -360 0 0 {name=l9 lab=G}
C {res.sym} 920 -200 0 0 {name=Rload m=1 value=100 footprint=1206 device=resistor
tedax_ignore=true}
C {gnd.sym} 920 -150 0 0 {name=l10 lab=VSS}
C {pnp.sym} 730 -390 0 0 {name=Q6 
model=BC857 
device=BC857 
footprint=SOT23
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=2ahUKEwijlfagu4zfAhUN0xoKHTPBAb0QFjAAegQIAhAC&url=http%3A%2F%2Fwww.onsemi.com%2Fpub%2FCollateral%2FPN2907-D.PDF&usg=AOvVaw2wgr87fGZgGfBRhXzHGwZM"}
C {zener.sym} 480 -190 2 0 {name=x3
model=BZX5V1 
device=BZX5V1 
footprint=acy(300)
xxxspiceprefix="#D#"}
C {lab_pin.sym} 480 -130 0 0 {name=l13 lab=VSSA}
C {res.sym} 480 -460 0 0 {name=Rdivider_upper m=1 value=4.7K footprint=1206 device=resistor}
C {res.sym} 750 -200 0 0 {name=R5 m=1 value=470 footprint=1206 device=resistor}
C {gnd.sym} 750 -150 0 0 {name=l16 lab=VSS}
C {lab_wire.sym} 510 -390 0 0 {name=l0 lab=B}
C {res.sym} 480 -340 0 0 {name=Rdivider_lower m=1 value=510 footprint=1206 device=resistor}
C {pmos.sym} 900 -360 0 0 {name=M2 
model=IRLML6402 
device=IRLML6402 
footprint=SOT23
spiceprefix=X
m=1
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&ved=2ahUKEwjs8pzxuozfAhWpz4UKHR4CDnMQFjAAegQIAhAC&url=https%3A%2F%2Fwww.infineon.com%2Fdgdl%2Firlml6402.pdf%3FfileId%3D5546d462533600a401535668c9822638&usg=AOvVaw21fCRax-ssVpLqDeGK8KiC"}
C {led.sym} 800 -290 0 0 {name=x1 model=D1N5765 device=D1N5765 area=1 footprint=acy(300)}
C {title.sym} 160 -30 0 0 {name=l2 author="Igor2"}
C {lab_pin.sym} 320 -340 0 1 {name=p6 lab=VOUT}
C {lab_pin.sym} 320 -360 0 1 {name=p7 lab=VSSA}
C {lab_pin.sym} 320 -380 0 1 {name=p8 lab=VCC_UNREG}
C {zener.sym} 400 -190 2 0 {name=x4 
model=BZX5V1 
device=BZX5V1
area=1 
footprint=minimelf 
spice_ignore=true}
C {lab_pin.sym} 400 -130 0 0 {name=l1 lab=VSSA}
C {res.sym} 700 -290 0 0 {name=R1 m=1 value=47K footprint=1206 device=resistor}
C {lab_wire.sym} 480 -260 0 0 {name=l3 lab=Z}
C {reg.sch} -120 -520 0 0 {name=x2}
C {ipin.sym} 170 -710 0 0 {name=p3 lab=VCC_UNREG}
C {ipin.sym} 170 -630 0 0 {name=p1 lab=VSSA}
C {lab_wire.sym} 690 -520 0 0 {name=l4 lab=VCC_FIVE}
C {diode.sym} 260 -770 0 1 {name=D1 model=D1N914 area=1 device=D1N914 footprint=acy(300)}
C {vdd.sym} 260 -800 0 0 {name=l5 lab=VCC}
