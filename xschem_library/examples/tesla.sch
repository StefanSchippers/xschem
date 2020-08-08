v {xschem version=2.9.5_RC8 file_version=1.1}
G {}
V {}
S {}
E {}
T {PARASITIC
CAP} 1020 -510 0 0 0.25 0.2 {}
T {TESLA COIL HIGH VOLTAGE GENERATOR EXAMPLE} 420 -740 0 0 0.4 0.4 {}
N 140 -550 140 -500 {lab=VCCS}
N 140 -640 140 -610 {lab=VCC}
N 230 -640 230 -500 {lab=VCC}
N 870 -530 1010 -530 {lab=HV}
N 1010 -420 1010 -250 {lab=VSS}
N 1010 -530 1010 -480 {lab=HV}
N 870 -340 870 -140 {lab=FB}
N 870 -580 870 -530 {lab=HV}
N 860 -580 870 -580 {lab=HV}
N 845 -590 860 -580 {lab=HV}
N 840 -600 845 -590 {lab=HV}
N 840 -610 840 -600 {lab=HV}
N 840 -620 845 -630 {lab=HV}
N 845 -630 860 -640 {lab=HV}
N 870 -640 880 -640 {lab=HV}
N 870 -580 880 -580 {lab=HV}
N 880 -580 895 -590 {lab=HV}
N 895 -590 900 -600 {lab=HV}
N 900 -610 900 -600 {lab=HV}
N 895 -630 900 -620 {lab=HV}
N 880 -640 895 -630 {lab=HV}
N 840 -610 860 -600 {lab=HV}
N 860 -600 880 -600 {lab=HV}
N 880 -600 900 -610 {lab=HV}
N 870 -640 880 -620 {lab=HV}
N 880 -600 880 -590 {lab=HV}
N 870 -580 880 -590 {lab=HV}
N 860 -620 870 -640 {lab=HV}
N 860 -600 860 -590 {lab=HV}
N 860 -590 870 -580 {lab=HV}
N 840 -620 860 -610 {lab=HV}
N 860 -610 880 -610 {lab=HV}
N 880 -610 900 -620 {lab=HV}
N 845 -630 860 -620 {lab=HV}
N 860 -620 880 -620 {lab=HV}
N 880 -620 895 -630 {lab=HV}
N 840 -600 860 -590 {lab=HV}
N 860 -590 880 -590 {lab=HV}
N 880 -590 900 -600 {lab=HV}
N 895 -630 907.5 -642.5 {lab=HV}
N 907.5 -642.5 907.5 -637.5 {lab=HV}
N 907.5 -637.5 930 -660 {lab=HV}
N 922.5 -655 930 -660 {lab=HV}
N 925 -652.5 930 -660 {lab=HV}
N 832.5 -642.5 845 -630 {lab=HV}
N 832.5 -642.5 832.5 -637.5 {lab=HV}
N 810 -660 832.5 -637.5 {lab=HV}
N 810 -660 817.5 -655 {lab=HV}
N 810 -660 815 -652.5 {lab=HV}
N 832.5 -577.5 845 -590 {lab=HV}
N 832.5 -582.5 832.5 -577.5 {lab=HV}
N 810 -560 832.5 -582.5 {lab=HV}
N 810 -560 817.5 -565 {lab=HV}
N 810 -560 815 -567.5 {lab=HV}
N 895 -590 907.5 -577.5 {lab=HV}
N 907.5 -582.5 907.5 -577.5 {lab=HV}
N 907.5 -582.5 930 -560 {lab=HV}
N 922.5 -565 930 -560 {lab=HV}
N 925 -567.5 930 -560 {lab=HV}
N 840 -620 840 -610 {lab=HV}
N 860 -640 870 -640 {lab=HV}
N 900 -620 900 -610 {lab=HV}
N 860 -610 860 -600 {lab=HV}
N 880 -610 880 -600 {lab=HV}
N 860 -620 860 -610 {lab=HV}
N 880 -620 880 -610 {lab=HV}
N 810 -430 810 -400 {lab=#net1}
N 810 -490 870 -490 {lab=HV}
N 580 -430 580 -400 {lab=#net2}
N 520 -430 520 -340 {lab=#net3}
N 520 -340 720 -340 {lab=#net3}
N 810 -340 870 -340 {lab=FB}
N 520 -490 580 -490 {lab=VCC}
N 580 -640 580 -490 {lab=VCC}
N 870 -530 870 -490 {lab=HV}
N 870 -430 870 -340 {lab=FB}
N 580 -170 580 -150 {lab=VSS}
N 140 -640 230 -640 {lab=VCC}
N 140 -420 230 -420 {lab=VSS}
N 800 -220 800 -200 {lab=VCC}
N 800 -140 870 -140 {lab=FB}
N 230 -440 230 -420 {lab=VSS}
N 140 -440 140 -420 {lab=VSS}
N 460 -200 540 -200 {lab=FB}
N 580 -280 580 -230 {lab=D}
N 230 -640 580 -640 {lab=VCC}
N 470 -280 470 -260 {lab=VCC}
N 460 -420 460 -400 {lab=VCC}
N 460 -340 520 -340 {lab=#net3}
N 720 -280 720 -260 {lab=VSS}
C {code.sym} 1140 -170 0 0 {
name=STIMULI 
value=".option SCALE=1e-6 PARHIER=LOCAL RUNLVL=6 post MODMONTE=1
.option sampling_method = SRS
.option method=gear
vvss vss 0 dc 0

.save all

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in simulation directory.
.include models_tesla.txt
.temp 30
.tran 1n 1m  uic
.meas tran iavg AVG i(vc) from=950u to=990u
.probe tran p(q1) i(d2) i(d1) i(d3) i(l1) i(l2) i(L3)
.meas tran pavg AVG p(q1) from=950u to=990u
"}
C {lab_pin.sym} 140 -640 0 0 {name=p1 lab=VCC}
C {res.sym} 140 -580 0 1 {name=Rs m=1 value=1}
C {vsource.sym} 140 -470 0 1 {name=Vvcc value="pwl 0 0 1u 12"}
C {lab_pin.sym} 140 -420 0 0 {name=p7 lab=VSS}
C {capa.sym} 230 -470 0 0 {name=Cvcc m=1 value=10u}
C {launcher.sym} 1180 -530 0 0 {name=h2 
descr="BJT MODELS" 
url="http://www.zen22142.zen.co.uk/ltspice/standard.bjt"}
C {launcher.sym} 1180 -480 0 0 {name=h1 
descr="NPN MODELS" 
url="http://web.mit.edu/Magic/Public/ckt/npn.mod"}
C {launcher.sym} 1180 -580 0 0 {name=h3 
descr="COIL CALCULATION" 
url="http://hamwaves.com/antennas/inductance.html"}
C {launcher.sym} 1180 -630 0 0 {name=h4 
descr="ltwiki BJT MODELS" 
url="http://ltwiki.org/?title=Standard.bjt"}
C {lab_pin.sym} 140 -530 0 0 {name=p6 lab=VCCS}
C {lab_pin.sym} 870 -560 0 1 {name=p4 lab=HV}
C {capa.sym} 1010 -450 0 0 {name=C1 m=1 value=1p}
C {lab_pin.sym} 1010 -250 0 0 {name=p3 lab=VSS}
C {ind.sym} 580 -460 2 1 {name=L1 value=9.8u}
C {ind.sym} 810 -460 0 1 {name=L2 value=9.1m}
C {k.sym} 700 -460 0 0 {name=K0 K=0.15 L1=L1 L2=L2}
C {res.sym} 810 -370 0 0 {name=R0 m=1 value=.32}
C {capa.sym} 870 -460 0 1 {name=C0 m=1 value=4.1p}
C {res.sym} 580 -370 0 1 {name=R3 m=1 value=0.22}
C {capa.sym} 520 -460 0 0 {name=C5 m=1 value=13p}
C {lab_pin.sym} 580 -260 0 1 {name=p5 lab=D}
C {lab_pin.sym} 870 -170 0 1 {name=p0 lab=FB}
C {diode.sym} 800 -110 2 0 {name=D2 model=d1n4148 area=1
}
C {lab_pin.sym} 580 -150 0 1 {name=p10 lab=VSS}
C {diode.sym} 800 -170 2 0 {name=D1 model=d1n4148 area=1
}
C {lab_pin.sym} 800 -220 0 1 {name=p16 lab=VCC}
C {lab_pin.sym} 460 -200 0 0 {name=p11 lab=FB}
C {lab_pin.sym} 800 -80 0 0 {name=p12 lab=VSS}
C {npn.sym} 560 -200 0 0 {name=Q5 model=bd139 area=1}
C {res.sym} 470 -230 0 0 {name=R1 m=1 value=100k}
C {lab_pin.sym} 470 -280 0 0 {name=p8 lab=VCC}
C {diode.sym} 460 -370 2 0 {name=D3 model=d1n5400 area=1
}
C {lab_pin.sym} 460 -420 0 0 {name=p13 lab=VCC}
C {ammeter.sym} 580 -310 0 0 {name=vmeasure}
C {diode.sym} 720 -310 2 0 {name=D4 model=d1n5400 area=1
}
C {lab_pin.sym} 720 -260 0 0 {name=p14 lab=VSS}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
