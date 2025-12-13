#!/usr/bin/gawk -f

BEGIN{

}

{
  debug=0
  if($1=="model") macro=$2

  if( $1=="inst") {
   props=""
   inst_nonets=1
   printf "%s ", expand_label(prefix[macro] $2)
  }

  if($1=="nets") {
    inst_nonets=0
    $1=""; $0 = $0
    n = split($0, s_arr)
    if(debug) print "---> " macro,  n
    for(i=1; i<=n; i++) {
      split(s_arr[i], ss_arr, "=")
      instport[ss_arr[1]] = ss_arr[2]
    }
    for(i=1; i<=n; i++) {
      if (! ((macro,i) in macroport)) {
        print "hercules2spice: " macro "," i " not in macroport" > "/dev/stderr"
      }
      if ( !(macroport[macro,i] in instport) ) {
        print "hercules2spice: " macroport[macro,i] " not in instport (" $0 ")" > "/dev/stderr"
      }
      s = expand_label(instport[ macroport[macro,i] ] )
      expand_label(macroport[macro,i])  # calculate label_width ...
      printf "@%d %s ", label_width,s
    }
    printf "%s %s\n", macro, props
  }

  if( $1 == "props" ) {
    $1=""; $0 = $0
    sub(/^ /, "")
    n = split($0, s_arr, /\\/)
    props=""
    for(i=1; i<=n; i+=2) {
      if( 1 ) {     ##############  s_arr[i] ~ /^[wWlL]$/) {
        if(props !="") props = props " "
        props = props s_arr[i] "=\"" s_arr[i+1] "\""
      }
    }
  }

  if($1 == "macro" || $1 == "primitive") {
    defaults=""
    if(inst_nonets) printf "%s %s\n", macro, props
    macroname = $3
    if($1=="macro") prefix[macroname] = "X"
    inputlist = outputlist = iolist = ""

    #### DIRTY FIXES , work around hercules netlist inconsistencies
    if( macroname =="pulseWidth") {
      inputlist="input A"
    }
    #### /DIRTY FIXES

    if(startmacro!="") print ".ends"
    printf  "\n.subckt %s ", $3
    startmacro=$1
  }

  if($1=="prefix") {
    prefix[macroname] = $2
  }

  if($1=="input") {
    inputlist=$0
    for(i=2; i<=NF;i++) {
      printf "%s ", expand_label($i)
    }
  }

  if($1=="output") {
    outputlist=$0
    for(i=2; i<=NF;i++) {
      printf "%s ", expand_label($i)
    }
  }

  if($1=="io") {
    iolist=$0
    for(i=2; i<=NF;i++) {
      printf "%s ", expand_label($i)
    }
  }

  if($1=="defaults") {
    defstring = $0
    sub(/^[ ]*defaults[ *]/,"",defstring)
    n = split(defstring, s_arr, /\\/)
    defaults=""
    for(i=1; i<=n; i+=2) {
      if(defaults !="") defaults = defaults " "
      defaults = defaults s_arr[i] "=\"" s_arr[i+1] "\""
    }

  }


  if( (startmacro=="primitive" && $0 ~/^$/) || (startmacro=="macro" && $1=="defaults") ) {
    pos=1
    printf " %s\n*.PININFO ", defaults
    n = split(inputlist, s_arr)
    for(i=2;i<=n;i++) {
      macroport[macroname,pos++] = s_arr[i]
      nn = split(expand_label(s_arr[i]),ss_arr, ",")
      for(j=1;j<=nn;j++) {
        printf "%s:I ", ss_arr[j]
      }
    }
    n = split(outputlist, s_arr)
    for(i=2;i<=n;i++) {
      macroport[macroname,pos++] = s_arr[i]
      nn = split(expand_label(s_arr[i]),ss_arr, ",")
      for(j=1;j<=nn;j++) {
        printf "%s:O ", ss_arr[j]
      }
    }
    n = split(iolist, s_arr)
    for(i=2;i<=n;i++) {
      macroport[macroname,pos++] = s_arr[i]
      nn = split(expand_label(s_arr[i]),ss_arr, ",")
      for(j=1;j<=nn;j++) {
        printf "%s:B ", ss_arr[j]
      }
    }
    macroport[macroname,"n"] = pos-1
    printf "\n"
  }



}

END{
  print ".ends"
}




function expand_label(s,               tmp, n, i, j, l, mult, s_arr, lab, idx_left, idx_right, incr, quote, str )
{
  if(debug) print "expand_label: " s


  # 20140324 replace commas within <> with "+"
  quote=0
  str=""
  for(i=1; i<= length(s); i++) {
    c = substr(s,i,1)
    if(c ~ /[<>]/) quote=!quote
    if(quote && c==",") str = str "+"
    else str = str c
  }
  s = str
  # /20140324

  ## <<<<<< need to handle labels like this: AAAA<2:3,5:8>
  ##                                         <*2>(AAA<2:3>,BBB<5:8>)



  l=""
  n = split(s,s_arr,/,/)
  for(i=1;i<=n;i++) {
    if(debug) print "expand_label: "  s_arr[i]
    if(s_arr[i] ~ /^<\*[0-9]+>/) {
      lab = mult = s_arr[i]
      sub(/<\*/,"",mult)
      sub(/>/,"",mult)
      sub(/^[^>]*>/,"",lab)
      mult+=0
      if(debug) print "expand_label: "  "mult=" mult
      for(j=0;j<mult;j++) {
        if(l!="") l = l ","
        l = l expand_label(lab)
      }
    } else if(s_arr[i] ~ /<[^>]+\+[^>]+>/) {
      lab = s_arr[i]
      sub(/<.*/,"",lab)
      tmp = index(s_arr[i], "+")
      if(l!="") l = l ","
      l = l expand_label( substr(s_arr[i], 1, tmp-1) ">")
      if(l!="") l = l ","
      l = l expand_label( lab "<" substr(s_arr[i], tmp+1))



    } else if(s_arr[i] ~ /^[^<>]+<[0-9]+:[0-9]+:[0-9]+>$/) {
      lab = incr = idx_left = idx_right = s_arr[i]
      sub(/^.*</,"",idx_left)
      sub(/:.*/,"",idx_left)
      sub(/^[^:]+:/,"",idx_right)
      sub(/:.*/,"",idx_right)
      sub(/^.*:.*:/,"",incr)
      sub(/>.*/,"",incr)
      sub(/<.*/,"",lab)
      if(debug) print "expand_label: "  "--> " ,idx_left, idx_right,incr,lab
      incr = sign(idx_right - idx_left)*incr

      for(j = idx_left; j * incr <= idx_right * incr; j+=incr) {
        if(l!="") l = l ","
        l = l lab "[" j "]"
        if(debug) print "expand_label: "  "j=", j
      }
    } else if(s_arr[i] ~ /^[^<>]+<[0-9]+:[0-9]+>$/) {
      lab = idx_left = idx_right = s_arr[i]
      sub(/^.*</,"",idx_left)
      sub(/:.*/,"",idx_left)
      sub(/^.*:/,"",idx_right)
      sub(/>.*/,"",idx_right)
      sub(/<.*/,"",lab)
      incr = sign(idx_right - idx_left)

      for(j = idx_left; j * incr <= idx_right * incr; j+=incr) {
        if(l!="") l = l ","
        l = l lab "[" j "]"
        if(debug) print "expand_label: "  "j=", j
      }
    } else {
      if(l!="") l = l ","
      l = l s_arr[i]
    }
  }
  if(debug) print "expand_label: " l
  label_width = split(l, s_arr, /,/)  ### GLOBAL VAR !!
  return l
}

function sign(a) { return a>=0? 1 : -1 }

