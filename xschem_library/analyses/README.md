# Visual simulation setup library

This library makes it possible to set up simulations in a way similar to that in Qucs. Each analysis is represented by a symbol. Symbols (e.g. `op.sym`, `ac.sym`, ...) have attributes that specify the analysis parameters. The instances of these symbols are netlisted as commands in the control block of VACASK/Ngspice. The ordering is specified by the `order` attribute. If `order` is not set, 0 is assumed.  The symbols (blocks) are netlisted in the ascending `order`. 

Each block displays only those attributes that are specified (i.e. the string length is>0). Netlisting is triggered by the `command_block.sym` block. In order for the netlister to generate the control block you have to place it in the schematic exactly once. In order for netlisting to work you have to source the `analyses.init.tcl` script at Xschem startup by adding the following line to xschemrc. 
```
source analyses.init.tcl
```

An example that demonstrates the capabilities of this library is in file `demo.sch`. 


# Setting up sweeps
A sweep is defined with an instance of the `sweep.sym` block. Each sweep has a `name` and a `tag` attribute. The `tag` is the name of the variable that will hold the swept values in the simulator's output file (applies only to VACASK). The `name` of the sweep is the one you have to refer to in an analysis to make sweep that particular analysis. To sweep an analysis set its `sweep` parameter to the `name` of the sweep that applies to that analysis. Each `sweep.sym` instance has a `sweep` parameter. This parameter makes it possible to chain sweeps and form multidimensional sweeps. 

For Ngspice sweeps are supported only for the operating point analysis where they are limited to a maximum of 2 dimensions. 


# Available analyses
The following analyses are supported

|Symbol     |VACASK          |Ngspice    |
|-----------|----------------|-----------|
|op.sym     |op              |op         |
|dc1d.sym   |1D sweep of op  |dc (1D)    |
|dcinc.sym  |dcinc           |-          |
|dcxf.sym   |dcxf            |tf         |
|ac.sym     |ac              |ac         |
|acxf.sym   |acxf            |-          |
|noise.sym  |noise           |noise      |
|tran.sym   |tran            |tran       |
|hb.sym     |hb              |-          |


# Verbatim blocks
To put verbatim text in a control block the `verbatim.sym` block can be used. The `simulator` attribute specifies the simulator for which the block will be netlisted. If the attribute is not specified the block is netlisted for all simulators. The contents of the block are specified by the `verbatim` attribute. 


# Postprocessing scripts
Postprocessing scripts can be invoked with instances of the the `postproc.sym` block. The `tool` attribute specifies the (double-quoted) external program to invoke. The `file` attribute specifies the (double-quoted) name of the file that will be passed to the tool. If unquoted `PYTHON` is specified as the `tool` the `python3` interpreter is invoked under Linux and `python.exe` is invoked under Windows. 

