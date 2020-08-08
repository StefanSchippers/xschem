#!/usr/bin/awk -f

BEGIN{
  print "v {xschem version=2.9.5_RC6 file_version=1.1}"
  print "G {}"
  print "V {}"
  print "S {}"
  print "E {}"

  command = "ls sym/*.sym"
  while(command |getline a) {
    sub(/.*\//, "",a)
    if(a ~/title/) continue


    x++
    if(x == 30) { y++; x=0}

    print "C {"  a "} " x*2000,y*2000, 0, 0, "{}"

   
  }
  close(command)

}



