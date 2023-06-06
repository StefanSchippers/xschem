xschem instance ../../../xschem_library/devices/nmos.sym  30 20  0   0    {name=MN1}
xschem instance ../../../xschem_library/devices/pmos.sym  30 -70 0   0    {name=MP1}
xschem instance ../../../xschem_library/devices/vdd.sym  50 -130 0   0    {name=VDD}
xschem instance ../../../xschem_library/devices/gnd.sym  50 80 0   0    {name=GND}
xschem instance ../../../xschem_library/devices/ipin.sym  -40 -70 0   0    {lab=in}
xschem instance ../../../xschem_library/devices/opin.sym  110 -20 0   0    {lab=out}
xschem wire 50 -40 50 -10
xschem wire 50 -20 110 -20
xschem wire 50 -130 50 -100
xschem wire 50 50 50 80
xschem wire -50 -70 10 -70
xschem wire -10 20 10 20
xschem wire -10 -70 -10 20 
xschem select_all
xschem hilight
xschem save
xschem exit closewindow