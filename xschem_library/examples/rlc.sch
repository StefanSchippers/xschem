v {xschem version=2.9.6 file_version=1.1}
G {}
V {}
S {}
E {}
N 280 -520 280 -420 {lab=B}
N 480 -460 480 -240 {lab=0}
N 280 -240 480 -240 {lab=0}
N 280 -360 280 -300 {lab=C}
N 280 -620 480 -620 {lab=A}
N 480 -620 480 -520 {lab=A}
N 280 -620 280 -580 {lab=A}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {launcher.sym} 320 -100 0 0 {name=h1
descr="NGSPICE Manual" 
url="https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=1&cad=rja&uact=8&ved=2ahUKEwiVvafVvtzfAhVHExoKHaZLAIQQFjAAegQICRAC&url=http%3A%2F%2Fngspice.sourceforge.net%2Fdocs%2Fngspice-manual.pdf&usg=AOvVaw3n8fjRjAJRgWMjg1Y04Ht8"}
C {code_shown.sym} 640 -210 0 0 {name=STIMULI
only_toplevel=true
tclcommand="xschem edit_vi_prop"
value="

.tran 10n 800u uic
.save all
"}
C {capa.sym} 280 -550 0 0 {name=C1 m=1 value=50nF footprint=1206 device="ceramic capacitor"}
C {lab_pin.sym} 280 -330 2 1 {name=l2 sig_type=std_logic lab=C}
C {lab_pin.sym} 280 -450 2 1 {name=l3 sig_type=std_logic lab=B}
C {lab_pin.sym} 480 -580 2 0 {name=l4 sig_type=std_logic lab=A}
C {lab_pin.sym} 480 -240 2 0 {name=l5 sig_type=std_logic lab=0}
C {res.sym} 480 -490 0 0 {name=R1 m=1 value=1k footprint=1206 device=resistor}
C {ind.sym} 280 -390 0 0 {name=L1 value=10mH}
C {vsource.sym} 280 -270 0 0 {name=V1 value="pwl 0 0 100u 0 101u 3"}
