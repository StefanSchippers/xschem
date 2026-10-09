v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
T {Widths of transistor set with xschem expr() function} -150 -290 0 0 0.6 0.6 {layer=7}
N -0 -190 -0 -90 {lab=VCC}
N 0 90 0 190 {lab=0}
N -0 -60 0 60 {lab=X1}
N -80 -90 -40 -90 {lab=IN}
N -80 -90 -80 90 {lab=IN}
N -80 90 -40 90 {lab=IN}
N 480 -190 480 -90 {lab=VCC}
N 480 90 480 190 {lab=0}
N 480 -60 480 60 {lab=OUT}
N 420 -90 440 -90 {lab=X3}
N 420 -90 420 90 {lab=X3}
N 420 90 440 90 {lab=X3}
N 480 0 590 0 {lab=OUT}
N -0 -190 480 -190 {lab=VCC}
N -0 190 480 190 {lab=0}
N 0 -0 120 0 {lab=X1}
N -140 -0 -80 -0 {lab=IN}
N 330 -190 330 -90 {lab=VCC}
N 330 90 330 190 {lab=0}
N 330 -60 330 60 {lab=X3}
N 270 -90 290 -90 {lab=X2}
N 270 -90 270 90 {lab=X2}
N 270 90 290 90 {lab=X2}
N 180 -190 180 -90 {lab=VCC}
N 180 90 180 190 {lab=0}
N 180 -60 180 60 {lab=X2}
N 120 -90 140 -90 {lab=X1}
N 120 -90 120 90 {lab=X1}
N 120 90 140 90 {lab=X1}
N 180 -0 270 -0 {lab=X2}
N 330 0 420 0 {lab=X3}
C {ipin.sym} -140 0 0 0 {name=p1 sig_type=std_logic lab=IN}
C {opin.sym} 590 0 0 0 {name=p2 sig_type=std_logic lab=OUT}
C {nmos4.sym} -20 90 0 0 {name=M1 model=cmosn 
w=@Wn_1x 
l=@nl 
del=0 
m=1}
C {pmos4.sym} -20 -90 0 0 {name=M2 model=cmosp 
w="expr(round(@mos_ratio * (@Wn_1x / @nl ) * @pl / 0.005) * 0.005)"
l=@pl 
del=0 m=1}
C {nmos4.sym} 460 90 0 0 {name=M3 model=cmosn 
w="expr(round(@Wn_1x * @stage_ratio ^3 / 0.005) * 0.005)"
l=@nl 
del=0 m=1}
C {pmos4.sym} 460 -90 0 0 {name=M4 model=cmosp 
w="expr(round(@mos_ratio * @stage_ratio ^3 * (@Wn_1x / @nl ) * @pl / 0.005) * 0.005)"
l=@pl 
del=0 m=1}
C {vdd.sym} 80 -190 0 0 {name=l1 lab=VCC}
C {gnd.sym} 80 190 0 0 {name=l2 lab=0}
C {nmos4.sym} 310 90 0 0 {name=M5 model=cmosn 
w="expr(round(@Wn_1x * @stage_ratio ^2 / 0.005) * 0.005)"
l=@nl 
del=0 m=1}
C {pmos4.sym} 310 -90 0 0 {name=M6 model=cmosp 
w="expr(round(@mos_ratio * @stage_ratio ^2 * (@Wn_1x / @nl ) * @pl / 0.005) * 0.005)"
l=@pl 
del=0 m=1}
C {nmos4.sym} 160 90 0 0 {name=M7 model=cmosn 
w="expr(round(@Wn_1x * @stage_ratio / 0.005) * 0.005)"
l=@nl 
del=0 m=1}
C {pmos4.sym} 160 -90 0 0 {name=M8 model=cmosp 
w="expr(round(@mos_ratio * @stage_ratio * (@Wn_1x / @nl ) * @pl / 0.005) * 0.005)"
l=@pl 
del=0 m=1}
C {lab_pin.sym} 120 -30 0 0 {name=p3 sig_type=std_logic lab=X1}
C {lab_pin.sym} 270 -30 0 0 {name=p4 sig_type=std_logic lab=X2}
C {lab_pin.sym} 420 -30 0 0 {name=p5 sig_type=std_logic lab=X3}
