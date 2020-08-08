#!/usr/bin/awk -f
# 


function expand_file(file,      err)
{
  while ((err=getline  < file) > 0)
  {
    if($1 ~ /^.?include$/) {
      expand_file($2)
    }
    else process_line()
  }
  if(err) {
    print "file: " file " not found." > "/dev/stderr"
    exit
  }
  close(file)
}


{
  if($1 ~ /^.?include$/)
  {
    expand_file($2)
  }
  else process_line()
}

function process_line(       a)
{
  if($0 ~ /^[ \t]*;/) { return }
  sub(/;/," ;",$0)
  if($3 ~/^%/){
    a=$0
    sub(/^[^%]*%/,"",a)
    sub(/%[^%]*$/,"",a)
    gsub(/ /,"_", a)
    sub(/%[^%]*%/, "%" a "%")
  }
  print
}



