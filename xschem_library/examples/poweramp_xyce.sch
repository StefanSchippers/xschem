v {xschem version=2.9.9 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
L 18 845 -530 880 -530 {}
L 18 845 -530 845 -450 {}
L 18 845 -450 880 -450 {}
L 18 880 -450 900 -400 {}
L 18 900 -400 910 -400 {}
L 18 910 -580 910 -400 {}
L 18 900 -580 910 -580 {}
L 18 880 -530 900 -580 {}
L 18 880 -530 880 -450 {}
L 18 900 -580 900 -400 {}
T {actual value
50u} 410 -820 0 0 0.4 0.4 {}
T {actual value
50u} 420 -360 0 0 0.4 0.4 {}
T {actual value
50u} 50 -750 0 0 0.4 0.4 {}
T {actual value
50u} 80 -290 0 0 0.4 0.4 {}
T {actual value
200} 870 -1330 0 0 0.4 0.4 {}
N 160 -1250 160 -1230 {lab=#net1}
N 160 -1110 160 -1090 {lab=#net2}
N 280 -1170 340 -1170 {lab=VSS}
N 710 -700 860 -700 {lab=OUTM}
N 510 -1200 580 -1200 {lab=VSS}
N 580 -1200 580 -1190 {lab=VSS}
N 580 -1190 620 -1190 {lab=VSS}
N 550 -1240 580 -1240 {lab=IN}
N 620 -1250 710 -1250 {lab=REFP}
N 280 -1180 280 -1170 {lab=VSS}
N 280 -1250 280 -1240 {lab=VPP}
N 270 -1100 270 -1090 {lab=VNN}
N 270 -1170 270 -1160 {lab=VSS}
N 160 -1090 190 -1090 {lab=#net2}
N 270 -1090 340 -1090 {lab=VNN}
N 160 -1250 190 -1250 {lab=#net1}
N 280 -1250 340 -1250 {lab=VPP}
N 270 -1170 280 -1170 {lab=VSS}
N 250 -1250 280 -1250 {lab=VPP}
N 250 -1090 270 -1090 {lab=VNN}
N 160 -1170 270 -1170 {lab=VSS}
N 550 -950 710 -950 {lab=OUTM}
N 400 -890 550 -890 {lab=FBN}
N 550 -770 550 -750 {lab=IN}
N 350 -890 350 -700 {lab=FBN}
N 710 -950 710 -700 {lab=OUTM}
N 710 -240 860 -240 {lab=OUTP}
N 260 -220 350 -220 {lab=INX}
N 550 -490 710 -490 {lab=OUTP}
N 400 -430 550 -430 {lab=FB}
N 350 -430 350 -240 {lab=FB}
N 710 -490 710 -240 {lab=OUTP}
N 240 -370 240 -350 {lab=VPP}
N 240 -290 240 -220 {lab=INX}
N 260 -220 260 -190 {lab=INX}
N 260 -130 260 -110 {lab=VSS}
N 510 -1120 580 -1120 {lab=VSS}
N 550 -1160 580 -1160 {lab=IN}
N 620 -1170 710 -1170 {lab=REFM}
N 200 -220 240 -220 {lab=INX}
N 550 -310 550 -290 {lab=VSS}
N 650 -700 710 -700 {lab=OUTM}
N 650 -240 710 -240 {lab=OUTP}
N 240 -220 260 -220 {lab=INX}
N 260 -680 350 -680 {lab=VSSX}
N 240 -830 240 -810 {lab=VPP}
N 240 -750 240 -680 {lab=VSSX}
N 260 -680 260 -650 {lab=VSSX}
N 260 -590 260 -570 {lab=VSS}
N 240 -680 260 -680 {lab=VSSX}
N 180 -680 240 -680 {lab=VSSX}
N 880 -1250 900 -1250 {lab=IN_INT}
N 880 -1250 880 -1220 {lab=IN_INT}
N 400 -1000 400 -980 {lab=VPP}
N 400 -920 400 -890 {lab=FBN}
N 400 -540 400 -520 {lab=VPP}
N 400 -460 400 -430 {lab=FB}
N 860 -700 860 -520 {lab=OUTM}
N 860 -460 860 -240 {lab=OUTP}
N 350 -890 400 -890 {lab=FBN}
N 350 -430 400 -430 {lab=FB}
N 580 -1110 620 -1110 {lab=VSS}
N 580 -1120 580 -1110 {lab=VSS}
C {code.sym} 1050 -580 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="* .option sampling_method = SRS 
* .option method=gear
vvss vss 0 dc 0 

.param frequ=20e3
.param gain=42
* .tran \{2e-3/frequ\} \{1e-3 + 160/frequ\} uic
.tran 6e-7 0.06 uic

** models are generally not free: you must download
** SPICE models for active devices and put them  into the below 
** referenced file in netlist/simulation directory.
.include \\"models_poweramp.txt\\"
* .save all
* .FOUR 20k v(outm,outp)
* .probe i(*) 
* .probe p(r*) p(v*)
"}
C {vsource.sym} 160 -1200 0 0 {name=V1 value="pwl 0 0 1m 50"}
C {vsource.sym} 160 -1140 0 0 {name=V0 value="pwl 0 0 1m 50"}
C {lab_pin.sym} 340 -1250 0 1 {name=p5 lab=VPP}
C {lab_pin.sym} 340 -1090 0 1 {name=p6 lab=VNN}
C {lab_pin.sym} 340 -1170 0 1 {name=p3 lab=VSS}
C {lab_pin.sym} 860 -240 0 1 {name=p14 lab=OUTP}
C {res.sym} 860 -490 0 1 {name=R1 m=1 value=8}
C {lab_pin.sym} 510 -1200 0 0 {name=p26 lab=VSS}
C {lab_pin.sym} 550 -1240 0 0 {name=p31 lab=IN}
C {vcvs.sym} 620 -1220 0 0 {name=E3 value='gain*0.99'}
C {lab_pin.sym} 710 -1250 0 1 {name=p32 lab=REFP}
C {capa.sym} 280 -1210 0 0 {name=C2 m=1 value="100u"}
C {capa.sym} 270 -1130 0 0 {name=C3 m=1 value="100u"}
C {res.sym} 220 -1250 1 1 {name=R11 m=1 value=0.3}
C {res.sym} 220 -1090 1 1 {name=R9 m=1 value=0.3}
C {res.sym} 550 -920 0 1 {name=R19 m=1 value='100k'}
C {res.sym} 550 -860 0 1 {name=R0 m=1 value="'100k/gain'"}
C {lab_pin.sym} 550 -750 0 0 {name=p108 lab=IN}
C {mos_power_ampli.sym} 500 -660 0 0 {name=x1}
C {lab_pin.sym} 350 -640 0 0 {name=p2 lab=VPP}
C {lab_pin.sym} 350 -620 0 0 {name=p4 lab=VNN}
C {lab_pin.sym} 860 -700 0 1 {name=p9 lab=OUTM}
C {mos_power_ampli.sym} 500 -200 0 0 {name=x0}
C {lab_pin.sym} 350 -180 0 0 {name=p12 lab=VPP}
C {lab_pin.sym} 350 -160 0 0 {name=p13 lab=VNN}
C {res.sym} 240 -320 0 1 {name=R6 m=1 value=100k}
C {lab_pin.sym} 240 -370 0 0 {name=p7 lab=VPP}
C {res.sym} 260 -160 0 1 {name=R7 m=1 value=100k}
C {lab_pin.sym} 260 -110 0 0 {name=p15 lab=VSS}
C {lab_pin.sym} 510 -1120 0 0 {name=p20 lab=VSS}
C {lab_pin.sym} 550 -1160 0 0 {name=p21 lab=IN}
C {vcvs.sym} 620 -1140 0 0 {name=E0 value='-gain*0.99'}
C {lab_pin.sym} 710 -1170 0 1 {name=p23 lab=REFM}
C {lab_pin.sym} 240 -250 0 0 {name=p8 lab=INX}
C {lab_pin.sym} 880 -1100 0 0 {name=p126 lab=VSS}
C {lab_pin.sym} 960 -1250 0 1 {name=p127 lab=IN}
C {capa.sym} 550 -800 0 0 {name=C5 m=1 value="100n ic=0"}
C {lab_pin.sym} 550 -290 0 0 {name=p11 lab=VSS}
C {capa.sym} 550 -340 0 0 {name=C6 m=1 value="100n ic=0"}
C {lab_pin.sym} 350 -200 0 0 {name=p28 lab=VSS}
C {lab_pin.sym} 350 -660 0 0 {name=p1 lab=VSS}
C {res.sym} 550 -460 0 1 {name=R2 m=1 value='100k'}
C {res.sym} 550 -400 0 1 {name=R3 m=1 value="'100k/(gain-2)'"}
C {vsource.sym} 880 -1190 0 0 {name=V3 
xvalue="pulse -.1 .1 1m .1u .1u 10.1u 20u" 
value="sin 0 1 20k 1m"
}
C {res.sym} 240 -780 0 1 {name=R4 m=1 value=100k}
C {lab_pin.sym} 240 -830 0 0 {name=p18 lab=VPP}
C {res.sym} 260 -620 0 1 {name=R5 m=1 value=100k}
C {lab_pin.sym} 260 -570 0 0 {name=p10 lab=VSS}
C {res.sym} 400 -950 0 1 {name=R8 m=1 value=100k}
C {capa.sym} 170 -220 1 0 {name=C4 m=1 value="100n ic=0"}
C {lab_pin.sym} 140 -220 0 0 {name=p0 lab=IN}
C {capa.sym} 150 -680 1 0 {name=C1 m=1 value="100n ic=0"}
C {lab_pin.sym} 120 -680 0 0 {name=p17 lab=VSS}
C {lab_pin.sym} 240 -710 0 0 {name=p22 lab=VSSX}
C {res.sym} 930 -1250 1 1 {name=R10 m=1 value=2}
C {lab_pin.sym} 400 -1000 0 0 {name=p24 lab=VPP}
C {res.sym} 400 -490 0 1 {name=R13 m=1 value=100k}
C {lab_pin.sym} 400 -540 0 0 {name=p16 lab=VPP}
C {vsource.sym} 880 -1130 0 0 {name=V4 value=0 xvalue="pwl 0 .1 1m .1 1.01m 0"
}
C {lab_pin.sym} 350 -270 0 0 {name=p19 lab=FB}
C {lab_pin.sym} 350 -730 0 0 {name=p25 lab=FBN}
C {title.sym} 160 -30 0 0 {name=l2 author="Stefan Schippers"}
C {lab_pin.sym} 880 -1250 0 0 {name=p27 lab=IN_INT}
C {launcher.sym} 990 -120 0 0 {name=h2
descr="Ctrl-Click
Clear all probes" 
tclcommand="
    xschem push_undo
    xschem set no_undo 1
    xschem set no_draw 1
 
    set lastinst [xschem get instances]
    for \{ set i 0 \} \{ $i < $lastinst \} \{incr i \} \{
      set type [xschem getprop instance $i cell::type]
      if \{ [regexp \{(^|/)probe$\} $type ] \} \{
        xschem setprop $i voltage fast
      \}
      if \{ [regexp \{current_probe$\} $type ] \} \{
        xschem setprop $i current fast
      \}
      if \{ [regexp \{differential_probe$\} $type ] \} \{
        xschem setprop $i voltage fast
      \}
    \}
    xschem set no_undo 0
    xschem set no_draw 0
    xschem redraw
"
}
C {launcher.sym} 990 -190 0 0 {name=h3
descr="Load file into gaw" 
comment="
  This launcher gets raw filename from current schematic using 'xschem get schname'
  and stripping off path and suffix.  It then loads raw file into gaw.
  This allow to use it in any schematic without changes.
"
tclcommand="
set rawfile [file tail [file rootname [xschem get schname]]].raw
gaw_cmd \\"tabledel $rawfile
load $netlist_dir/$rawfile
table_set $rawfile\\"
unset rawfile"
}
C {spice_probe.sym} 730 -700 0 0 {name=p40 analysis=tran}
C {spice_probe.sym} 740 -240 0 0 {name=p41 analysis=tran}
C {spice_probe.sym} 670 -1250 0 0 {name=p42 analysis=tran}
C {spice_probe.sym} 680 -1170 0 0 {name=p43 analysis=tran}
C {spice_probe.sym} 960 -1250 0 0 {name=p44 analysis=tran}
