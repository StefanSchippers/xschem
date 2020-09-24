proc gtkwave_getdata {sock} {
  global gtkwave_server_getdata
  if {[eof $sock] || [catch {gets $sock gtkwave_server_getdata(line,$sock)}]} {
    close $sock
    puts "Close $gtkwave_server_getdata(addr,$sock)"
    unset gtkwave_server_getdata(addr,$sock)
    unset gtkwave_server_getdata(line,$sock)
    unset gtkwave_server_getdata(res,$sock)
  } else {
    puts "tcp--> $gtkwave_server_getdata(line,$sock)"
    # gtkwave command must be executed at global scope...
    uplevel #0 [list catch $gtkwave_server_getdata(line,$sock) gtkwave_server_getdata(res,$sock)]
    puts $sock "$gtkwave_server_getdata(res,$sock)"
  }
}

proc gtkwave_server {sock addr port} {
  global gtkwave_server_getdata
  puts "Accept $sock from $addr port $port"
  fconfigure $sock -buffering line
  set gtkwave_server_getdata(addr,$sock) [list $addr $port]
  fileevent $sock readable [list gtkwave_getdata $sock]
}

set gtkwave_port 2022
socket -server gtkwave_server $gtkwave_port

