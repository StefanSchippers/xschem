# Visual simulation setup library

This library makes it possible to set up simulations in a way similar to that in Qucs. Each analysis is represented by a symbol. Symbols (e.g. `op.sym`, `ac.sym`, ...) have attributes that specify the analysis parameters. The instances of these symbols are netlisted as commands in the control block of VACASK/Ngspice. The ordering is specified by the `order` attribute. If `order` is not set, 0 is assumed.  The symbols (blocks) are netlisted in the ascending order based on the `order` atttribute. 

Each block displays only those attributes that are specified (i.e. the string length is>0). Netlisting is triggered by the `command_block.sym` block. In order for the netlister to generate the control block you have to place it in the schematic exactly once. In order for netlisting and symbol rendering to work you have to source the `analyses.init.tcl` script at Xschem startup by adding the following line to xschemrc. 
```
append postinit_commands {
  foreach i $pathlist {
    if {![catch {source $i/lib_init.tcl} retval]} {
      puts "Sourced library init file $i/lib_init.tcl"
    } 
  }
}
```
This will source all `lib_init.tcl` files in all libraries. 

An example that demonstrates the capabilities of this library is in file `demo.sch`. 


# Setting up sweeps

A sweep is defined with an instance of the `sweep.sym` block. Each sweep has a `name` and a `tag` attribute. The `tag` is the name of the variable that will hold the swept values in the simulator's output file (applies only to VACASK). The `name` of the sweep is the one you have to refer to in an analysis to make sweep that particular analysis. To sweep an analysis set its `sweep` parameter to the `name` of the sweep that applies to that analysis. Each `sweep.sym` instance has a `sweep` parameter. This parameter makes it possible to chain sweeps and form multidimensional sweeps. 

For Ngspice sweeps are supported only for the operating point analysis where they are limited to a maximum of 2 dimensions. 


# Available analyses

The following analyses are supported

|Symbol    |Analysis                          |VACASK      |Ngspice |
|----------|----------------------------------|------------|--------|
|op.sym    |operating point                   |op          |op      |
|dc1d.sym  |1D operating point sweep          |1D sweep+op |dc (1D) |
|dcinc.sym |small signal dc                   |dcinc       |-       |
|dcxf.sym  |small signal dc transfer function |dcxf        |tf      |
|ac.sym    |small signal ac                   |ac          |ac      |
|acxf.sym  |small signal ac transfer function |acxf        |-       |
|noise.sym |small signal noise                |noise       |noise   |
|tran.sym  |transient                         |tran        |tran    |
|hb.sym    |harmonc balance                   |hb          |-       |


# Verbatim blocks

To put verbatim text in a control block the `verbatim.sym` block can be used. The `simulator` attribute specifies the simulator for which the block will be netlisted. If the attribute is not specified the block is netlisted for all simulators. The contents of the block are specified by the `verbatim` attribute. 


# Sweeps and quoting

If you want to sweep an analysis, set its `sweep` attribute to the name of the sweep instance (`sweep.sym`). Nested sweeps are specified by setting the `sweep` attribute of a sweep instance. Ngspice has only limited sweeping support (maximal sweep depth is 2, sweeps are allowed only in operating point analysis, you cannot sweep arbitrary parameters). 

All attribute values must be specified in double quotes. Some attributes values must include quotes (must be quoted). When quoting attributes the quotes must be escaped with a backslash (`\"`). The `name` and the `sweep` attribute should not be quoted. All other string attributes must be quoted. The `ic` and `nodeset` attributes (VACASK netlists) should be quoted if they represent the name of the store with stored operating point solution. When explicitly specifying nodesets and initial conditions these attributes should not be quoted. 


# Specifying outputs

For VACASK netlists differential outputs (dcxf, acxf, and noise analyses) are specified with the `outp` and `outn` attributes. These attributes are strings and must be quoted. If the output is a current, specify its name (i.e. `vname.flow(br)`) under `outp` and leave `outn` empty. In both cases `outp` and `outn` values must be quoted. 

For Ngspice netlists differential outputs can be specified either in the same way as for VACASK. An alternative is to specify the output descriptor in SPICE3 style with `outp` (`v(a)`, `v(a,b)`, or `i(vname)`) while leaving `outn` empty. In this case do not quote the value of `outp`. 


# Postprocessing scripts

Postprocessing scripts can be invoked with instances of the the `postproc.sym` block. The `tool` attribute specifies the quoted external program to invoke. The `file` attribute specifies the quoted name of the file that will be passed to the tool. If `tool` is not quoted and its value is `PYTHON` the `python3` interpreter is invoked under Linux and `python.exe` is invoked under Windows. 


# Ngspice netlisting notes

Nodesets and initial conditions specified with the `nodeset` and `ic` attributes are ignored. Nodesets and initial conditions must be specified with ˙.nodeset˙ and ˙.ic˙ directives in a verbatim block. 

A dcxf block runs the TF analysis. The value of the `in` attribute specifies the quoted name of the input source. This attribute is ignored in VACASK netlists because VACASK computes the transfer functions from all independent sources to the given output. 

For the noise block the number of points per summary can be set with the `ptssum` attribute. This attribute is ignored in VACASK netlists. 
