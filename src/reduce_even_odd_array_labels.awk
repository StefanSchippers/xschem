#!/usr/bin/awk -f
# in .sch and/or .sym XSCHEM files
# reduce labels of type AA[7],AA[5],AA[3],AA[1] to AA[7:1:2]
# usage: reduce_even_odd_array_labels.awk *.sym *.sch
# stefan, 20161226

FNR == 1 {
    if (_filename_ != "")
        endfile(_filename_)
    _filename_ = FILENAME
    #print "processing: " FILENAME >"/dev/stderr"
    beginfile(FILENAME)
}

END  { endfile(_filename_) }

###### begin user code ########################

# C {devices/iopin} 270 60 0 0  {name=p311557 lab=AABLEQBL[30],AABLEQBL[28
/^C .devices.*pin\}/{
  l=$0
  sub(/^.*lab=/,"", l)
  sub(/\} *$/,"", l)
  translate()
  if(evenbus) {
    found=1
    sub(/lab=.*/,"", $0)
    $0 = $0 " lab=" newlab "} "
  }
}


# T {nDisturbL[31],nDisturbL[29],nDi..... } 125 1686 0 1 0.2 0.2 {} 
# B 5 147.5 -1672.5 152.5 -1667.5 {name=ARBLEQALF[30],ARBLEQALF[28],...,ARBLEQALF[0] dir=inout }


/^T \{/{
  l=$2
  sub(/\{/,"", l)
  sub(/\}$/,"", l)
  translate()
  if(evenbus) {
    found=1
    $2="{" newlab "}"
    $0=$0
  }
}

/^B /{
  l=$7
  sub(/\{name=/,"", l)
  translate()
  if(evenbus) {
    found=1
    $7="{name=" newlab
    $0=$0
  }
}

function translate()
{
  evenbus=0
  nn = split(l,ll,",")
  if(nn>1) {
    evenbus=1
    for(ii=1;ii<=nn;ii++) {
      basename=ll[ii]
      sub(/\[.*/,"",basename)
      if(ii==1) {
        first_basename = basename
      }
      idx = ll[ii]
      sub(/.*\[/,"",idx)
      sub(/\]/,"",idx)
      idx+=0
   
      if(ii>1 && ( (idx+2!=previdx) || (basename !=first_basename) ) ) {
        evenbus=0
        break
      }
      previdx = idx+0
    }
    if(evenbus) {
      idxlow = idx
      idxhi=idxlow+2*(nn-1)
      newlab = basename "[" idxhi ":" idxlow ":" 2 "]"
    }
  }
}
###### end  user code  ########################


{
 __a[__lines++] = $0 
}

function beginfile(f)
{
 __lines=0
 found=0
}

function endfile(f,     i)
{
 if(found)  {
   print "patching: " f >"/dev/stderr"
   for(i=0;i<__lines;i++)
   {
    print __a[i] > f
   }
   close(f)
 }
} 

