#!/usr/bin/gawk -f
# replaces _ALLOC_ID_ in all source files with unique ID for memory tracking

BEGIN{

  if(ARGC <= 1) {
     print "Usage: " ENVIRON["_"] " create|reset"
     exit
  }
  create_id = 0
  if(ARGV[1] == "create") create_id = 1
  ARGC = 1
  cnt = 0;
  while("ls *.[chyl]" |getline) {
    if($0 ~ /expandlabel\.c/) continue
    if($0 ~ /expandlabel\.h/) continue
    if($0 ~ /parselabel\.c/) continue
    filename = $0
    f = ""
    start = 1
    changed = 0
    while(getline < filename) {
      if(create_id == 0) {
        if(!start) f = f "\n"
        start = 0
        str = gensub(/(my_(malloc|calloc|realloc|free|strdup|strdup2))\([0-9]+,/, "\\1(_ALLOC_ID_,", "G")
        if(str != $0) changed = 1
        f = f str
      } else {
        if(!start) f = f "\n"
        start = 0
        if($0 !~ /^#define *_ALLOC_ID_/) {
          while(sub(/_ALLOC_ID_/, cnt)) {
            changed = 1
            cnt++
          }
        }
        f = f $0
      }
    }
    close(filename)
    if(changed) {
      print f > filename
      close(filename)
    }
  }
}
