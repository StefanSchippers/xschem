#!/bin/sh
# viewdraw to xschem all-file translate script
# convert all examples in this test directory, put all results in current directory
# usage: 
#   ./translate.sh

# process all files in sym/ directory
for i in $(ls test2/sym/*.1)
do
  j=$(basename -s .1 $i) # remove path and extension
  j=$(echo $j | tr '[:upper:]' '[:lower:]').sym #convert to lowercase, append .sym
  # run the translator. 1st param is the ViewDraw file, 2nd param the Xschem reference library
  ./viewdraw_import.awk $i xschem_lib > xschem_lib/$j
done

# process all files in sch/ directory
for i in $(ls test2/sch/*.1)
do
  j=$(basename -s .1 $i) # remove path and extension
  j=$(echo $j | tr '[:upper:]' '[:lower:]').sch #convert to lowercase, append .sch
  # run the translator. 1st param is the ViewDraw file, 2nd param the Xschem reference library
  ./viewdraw_import.awk $i xschem_lib > xschem_lib/$j
done

./viewdraw_import.awk rotation.1 xschem_lib > xschem_lib/rotation.sch
./viewdraw_import.awk align.1 xschem_lib > xschem_lib/align.sch
