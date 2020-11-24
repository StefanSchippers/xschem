#!/usr/bin/awk -f

BEGIN{
  first = 1
  hier = 0
  debug = 0
  hiersep="."
}

/^begin netlist/{
  cell = $4
  if(first) {
    topcell=cell
    first = 0
  }
  start[cell] = NR
}

/^end netlist/{
  end[cell] = NR
}
/^__GLOBAL__/{
  global[$2] = 1
}

/^__HIERSEP__/{
  hiersep = $2
}

{
  netlist[NR] = $0
}

END{
  print "tEDAx v1"
  print "begin netlist v1 " topcell
  expand(topcell, "", "", "")
  print "end netlist"
}


function expand(cell, instname, path, maplist,       i, j, subpos, subcell, subinst, submaplist)
{
  hier++
  for(i = start[cell]+1; i <= end[cell]-1; i++) {
    $0 = netlist[i]
    # __subcircuit__ pcb_voltage_protection x0 
    # __map__ VOUT -> VOUTXX
    # __map__  ... -> ...
    if($1 == "__subcircuit__") {
      subcell=$2
      subinst=$3
      subpos = i
      sub(/__subcircuit__/, "subcircuit", $0)
      print "#" spaces(hier * 2 - 1) $0
      for(i++; ;i++) {
        $0 = netlist[i]
        if($1 != "__map__") break
        dbg("expand: $4=" $4)
        $4 = resolve_node($4, path, maplist)
        submaplist = submaplist " " $2 " " $4
        sub(/__map__/, "map", $0)
        print "#" spaces(hier * 2 - 1) $0
      }
      expand(subcell, subinst, path subinst hiersep, submaplist)
    }
    if($1 == "conn") {
      dbg("conn: $2=" $2)
      $2 = resolve_node($2, path, maplist) #net name
      $3 = path $3 #refdes
    }
    if($1 ~/^(device|footprint|value|spiceval|spicedev|comptag|pinname|pinslot|pinidx)$/) {
      $2 = path $2 #refdes
    }
    print spaces(hier * 2) $0
    if($0 ~ /^end netlist/) break
  }
  hier--
}

function spaces(n,     s)
{
  n = (n > 0) ? n - 1 : 0 
  s ="                        "
  while(length(s) < n) s = s s
  return substr(s,1,n)
}

function dbg(s)
{
  if(debug) print s > "/dev/stderr"
}

function resolve_node(node, path, maplist,     arr, n, retnode, i) 
{
  dbg("resolve_node: node=" node " maplist=" maplist)
  n = split(maplist, arr)
  if(node in global) retnode = node
  else for(i = 1; i <= n; i += 2) {
    if(node == arr[i]) {
      retnode = arr[i+1]
      break
    }
  }
  if(retnode =="") retnode = path node
  return retnode
}
