#!/bin/sh

awk '
BEGIN{
 pattern = "(my_free|my_malloc|my_realloc|my_calloc|my_strdup|my_strndup|my_strdup2|my_mstrcat|my_strcat|my_strncat)\\("
}


$0 ~ pattern "[0-9]+" {
  if($0 !~/^ *\/\*/) {
    a = $0
    sub("^.*" pattern,"", a)
    sub(/ *,.*/,"", a)
    print a "\t" FILENAME "\t" $0
  }
}
'  \
actions.c \
callback.c \
check.c \
clip.c \
draw.c \
editprop.c \
expandlabel.y \
findnet.c \
font.c \
globals.c \
hash_iterator.c \
hilight.c \
icon.c \
in_memory_undo.c \
main.c \
move.c \
netlist.c \
node_hash.c \
options.c \
parselabel.l \
paste.c \
psprint.c \
rawtovcd.c \
save.c \
scheduler.c \
select.c \
spice_netlist.c \
store.c \
svgdraw.c \
tedax_netlist.c \
token.c \
verilog_netlist.c \
vhdl_netlist.c \
xinit.c \
| sort -n \
| awk '{
  if(n>0 && $1 <= prev)         print ">>>>>>>>>>> ERROR >>>>>>>>>"
  else  if(n++>0 && $1 != prev+1) print ">>>>>>>>>>>>>>>>>>>>>>>>>>>"
  prev = $1
  print
}'
