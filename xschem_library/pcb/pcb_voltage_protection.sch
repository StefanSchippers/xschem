v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
N 770 -490 770 -390 {lab=VCC}
N 600 -360 730 -360 {lab=G}
N 770 -300 860 -300 {lab=VOUT}
N 770 -160 770 -130 {lab=VSS}
N 770 -300 770 -220 {lab=VOUT}
N 330 -310 330 -220 {lab=Z}
N 330 -390 560 -390 {lab=B}
N 330 -160 330 -130 {lab=VSS}
N 330 -430 330 -390 {lab=B}
N 600 -490 600 -420 {lab=VCC}
N 600 -260 600 -220 {lab=#net1}
N 600 -160 600 -130 {lab=VSS}
N 330 -390 330 -370 {lab=B}
N 770 -330 770 -300 {lab=VOUT}
N 600 -360 600 -320 {lab=G}
N 250 -160 250 -130 {lab=VSS}
N 250 -220 330 -220 {lab=Z}
N 550 -320 650 -320 {lab=G}
N 550 -260 650 -260 {lab=#net1}
C {conn_3x1.sym} 150 -360 0 0 {name=C1}
C {vdd.sym} 770 -490 0 0 {name=l6 lab=VCC}
C {lab_pin.sym} 860 -300 0 1 {name=p0 lab=VOUT}
C {lab_wire.sym} 660 -360 0 0 {name=l9 lab=G}
C {res.sym} 770 -190 0 0 {name=Rload m=1 value=100 footprint=1206 device=resistor
tedax_ignore=true}
C {gnd.sym} 770 -130 0 0 {name=l10 lab=VSS}
C {code_shown.sym} 950 -430 0 0 {name=STIMULI
tedax_ignore=true
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="

.option PARHIER=LOCAL RUNLVL=5 post MODMONTE=1 warn maxwarns=400 ingold=1

vvcc vcc 0 dc 0 pwl 0 0 1m 6 2m 6 3m 5 4m 5 5m 0
vvss vss 0 dc 0
* .tran 5u 7m uic

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in netlist/simulation directory.
.include \\"models_pcb_voltage_protection.txt\\"
.dc VVCC 0 8 0.004
.save all
"}
C {pnp.sym} 580 -390 0 0 {name=Q6 
model=BC857 
device=BC857 
footprint=SOT23
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=2ahUKEwijlfagu4zfAhUN0xoKHTPBAb0QFjAAegQIAhAC&url=http%3A%2F%2Fwww.onsemi.com%2Fpub%2FCollateral%2FPN2907-D.PDF&usg=AOvVaw2wgr87fGZgGfBRhXzHGwZM"}
C {zener.sym} 330 -190 2 0 {name=x3
model=BZX5V1 
device=BZX5V1 
footprint=acy(300)
xxxspiceprefix="#D#"}
C {gnd.sym} 330 -130 0 0 {name=l13 lab=VSS}
C {res.sym} 330 -460 0 0 {name=R4 m=1 value=4.7K footprint=1206 device=resistor}
C {vdd.sym} 330 -490 0 0 {name=l14 lab=VCC}
C {vdd.sym} 600 -490 0 0 {name=l15 lab=VCC}
C {res.sym} 600 -190 0 0 {name=R5 m=1 value=470 footprint=1206 device=resistor}
C {gnd.sym} 600 -130 0 0 {name=l16 lab=VSS}
C {lab_wire.sym} 360 -390 0 0 {name=l0 lab=B}
C {res.sym} 330 -340 0 0 {name=R2 m=1 value=510 footprint=1206 device=resistor}
C {pmos.sym} 750 -360 0 0 {name=M2 
model=IRLML6402 
device=IRLML6402 
footprint=SOT23
spiceprefix=X
m=1
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&ved=2ahUKEwjs8pzxuozfAhWpz4UKHR4CDnMQFjAAegQIAhAC&url=https%3A%2F%2Fwww.infineon.com%2Fdgdl%2Firlml6402.pdf%3FfileId%3D5546d462533600a401535668c9822638&usg=AOvVaw21fCRax-ssVpLqDeGK8KiC"}
C {led.sym} 650 -290 0 0 {name=x1 model=D1N5765 device=D1N5765 area=1 footprint=acy(300)}
C {title.sym} 160 -30 0 0 {name=l2 author="Igor2"}
C {lab_pin.sym} 170 -340 0 1 {name=p6 lab=VOUT}
C {lab_pin.sym} 170 -360 0 1 {name=p7 lab=VSS}
C {lab_pin.sym} 170 -380 0 1 {name=p8 lab=VCC}
C {zener.sym} 250 -190 2 0 {name=x4 
model=BZX5V1 
device=BZX5V1
area=1 
footprint=minimelf 
spice_ignore=true}
C {gnd.sym} 250 -130 0 0 {name=l1 lab=VSS}
C {res.sym} 550 -290 0 0 {name=R1 m=1 value=47K footprint=1206 device=resistor}
C {lab_wire.sym} 330 -260 0 0 {name=l3 lab=Z}
