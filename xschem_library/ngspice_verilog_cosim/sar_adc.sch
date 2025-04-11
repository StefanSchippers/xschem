v {xschem version=3.4.7RC file_version=1.2}
G {}
K {}
V {}
S {}
E {}
P 4 7 1050 -570 1050 -530 1055 -530 1050 -520 1045 -530 1050 -530 1050 -570 {fill=full}
T {Analog conversion for plotting} 750 -110 0 0 0.4 0.4 {}
T {Change device_model attribute
to simulate this verilog block 
with Icarus Verilog or Verilator} 830 -645 0 0 0.4 0.4 {layer=4}
T {Array of 6 instances taking 6 different
C parameters. Notice the ?1 leading 
string in the C attribute: this tells
each instance takes one parameter.
In 99% of the cases you should always
use ?1 before the comma separated list
of values.
These comma separated values should be
separated by white space in netlist 
when unrolling the vector instance:
format="@name @pinlist @symname C = @C"
and not:
format="@name @pinlist @symname C=@C"
to avoid confusing the netlister.} 250 -800 0 0 0.4 0.4 {}
N 420 -210 420 -190 {lab=0}
N 860 -180 890 -180 {lab="%vd(TEST_V VREF)"}
N 950 -180 980 -180 {lab=COMP}
N 180 -540 180 -480 {lab=IIN}
N 180 -420 180 -380 {lab=TEST_V}
N 180 -380 360 -380 {lab=TEST_V}
N 180 -270 420 -270 {lab=TEST_V}
N 180 -380 180 -270 {lab=TEST_V}
C {ipin.sym} 80 -180 0 0 {name=p1 lab=INPUT}
C {ipin.sym} 80 -160 0 0 {name=p2 lab=VREF}
C {ipin.sym} 80 -140 0 0 {name=p3 lab=START}
C {opin.sym} 170 -140 0 0 {name=p4 lab=VALID}
C {ipin.sym} 80 -100 0 0 {name=p5 lab=CLK}
C {opin.sym} 170 -120 0 0 {name=p6 lab=D[5..0]}
C {capa.sym} 420 -240 0 0 {name=CLAST
m=1
value="'1p / 32'"
footprint=1206
device="ceramic capacitor"}
C {lab_pin.sym} 420 -190 0 0 {name=p26 lab=0}
C {adc_bridge.sym} 920 -180 0 0 {name=ACOMP adc_bridge_model= comparator

device_model=".model comparator adc_bridge in_low=0 in_high=0"}
C {lab_pin.sym} 860 -180 0 0 {name=p27 lab="%vd(TEST_V VREF)"}
C {lab_pin.sym} 980 -180 0 1 {name=p28 lab=COMP}
C {tgate.sym} 120 -540 0 0 {name=XSAMPLE}
C {lab_pin.sym} 60 -540 0 0 {name=p29 lab=INPUT}
C {lab_pin.sym} 180 -540 0 1 {name=p30 lab=IIN}
C {lab_pin.sym} 60 -580 0 0 {name=p31 lab=SAMPLE}
C {lab_pin.sym} 60 -620 0 0 {name=p32 lab=VREF}
C {res.sym} 180 -450 0 0 {name=R1
value=1k
footprint=1206
device=resistor
m=1}
C {lab_pin.sym} 180 -400 0 1 {name=p33 lab=TEST_V}
C {sar_adc_vlog.sym} 990 -440 0 0 {name=ADUT model=dut

***Icarus_verilog***
device_model=".model dut d_cosim simulation=\\"ivlng\\" sim_args=[\\"adc\\"]"

***Verilator***
*device_model=".model dut d_cosim simulation=\\"./adc.so\\""

tclcommand="edit_file [abs_sym_path adc.v]"
}
C {lab_pin.sym} 830 -480 0 0 {name=p7 lab=CLK}
C {lab_pin.sym} 830 -440 0 0 {name=p10 lab=COMP}
C {lab_pin.sym} 1150 -440 0 1 {name=p13 lab=VALID}
C {lab_pin.sym} 1150 -400 0 1 {name=p16 lab=~D[5..0]}
C {lab_pin.sym} 1150 -480 0 1 {name=p19 lab=SAMPLE}
C {lab_pin.sym} 830 -400 0 0 {name=p22 lab=START}
C {dac_bridge.sym} 900 -70 0 0 {name=A1 dac_bridge_model= dac_buff
device_model=".model dac_buff dac_bridge input_load=1e-15 t_rise=10n t_fall=10n
+ out_low=0 out_high=3.3"}
C {lab_pin.sym} 870 -70 0 0 {name=p34 lab=COMP}
C {lab_pin.sym} 930 -70 0 1 {name=p35 lab=COMP_A}
C {noconn.sym} 170 -120 0 0 {name=l1}
C {noconn.sym} 950 -180 3 0 {name=l2}
C {noconn.sym} 860 -180 3 0 {name=l3}
C {ccap.sym} 420 -360 0 0 {name=xb[6..1] C="?1 1p,'1p/2','1p/4','1p/8','1p/16','1p/32'"}
C {lab_pin.sym} 360 -360 0 0 {name=p36 lab=VREF}
C {lab_pin.sym} 360 -340 0 0 {name=p37 lab=D[5..0]}
