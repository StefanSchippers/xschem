# xschem
A schematic editor for VLSI/Asic/Analog custom designs, netlist backends for VHDL, Spice and Verilog. The tool is focused on hierarchy and parametric designs, to maximize circuit reuse.

[Manual and instructions](http://repo.hu/projects/xschem/xschem_man/xschem_man.html)

## Development

### Tcl Code Linting and Formatting

This project uses [tclint](https://github.com/nmoroze/tclint) for linting and formatting Tcl files.

To install tclint:
```bash
pip install tclint
```

To check Tcl code formatting:
```bash
tclfmt --check --exclude "src/xschem.tcl" .
```

To automatically format Tcl files:
```bash
tclfmt --in-place --exclude "src/xschem.tcl" .
```

To run the linter:
```bash
find . -name "*.tcl" -type f ! -path "./src/xschem.tcl" -exec tclint -c .tclint.toml {} \;
```

The configuration for tclint can be found in `.tclint.toml`.

