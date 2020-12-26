#!/usr/bin/awk -f

BEGIN{
 for(i=0;i<=127;i++)
 {
  val[sprintf("%c",i)]=i  # value of char: val["0"]=45
  char[i]=sprintf("%c",i) # char[48]="0"
 }
 lines=0
}
# LDYMS[12]

{ 
  line[lines++] = $0 " " hash_string($1)
}


END{
  close(FILENAME) 

  for(i=0;i< lines;i++) {
    print line[i] > FILENAME
  }
  close(FILENAME)
  system("sort -k 2 " FILENAME " | awk '{$NF=\"\";  print $0}' > "  FILENAME ".xxxxx")
  system("mv " FILENAME ".xxxxx " FILENAME)
}


function hash_string(s,         n, i, h, c) 
{
  n = s
  sub(/^.*\[/,"",n)
  sub(/\].*/,"",n)
  sub(/\[.*/, "", s) 

  h = s "__" sprintf("%08d", n)

  return h
}
