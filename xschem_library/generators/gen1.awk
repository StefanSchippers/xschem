#!/usr/bin/awk -f

BEGIN{
 header()
 print "L 6 0 0 100 100 {" ARGV[1] " " ARGV[2] "}"
}


function header()
{
  print "v {xschem version=3.1.0 file_version=1.2}"
  print "G {}"
  print "K {}"
  print "V {}"
  print "S {}"
  print "E {}"
}

