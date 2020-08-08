#!/usr/bin/awk -f
# 
#  plugin for clock expansion
#
# clock <signal> <pulse> <period> [invert] [high value] [low value]
#
#                    _______              _______
#___________________|       |____________|       |__________
#                   |       |            |
#                   |<pulse>|            |
#                   |<----- period ----->|     
#                   |
#             clock_start
#				Stefan, 04122001



BEGIN {
  OFMT="%.14g" # better precision
  CONVFMT="%.14g"
  old_event=-1
} 

## stefan fix 20100630: reset absolute time if multiple beginfile--endfile given
/^[ \t]*beginfile[ \t]+/{
  time=0
}

/^[ \t]*clock[ \t]+/{
  
 sub(/[ \t]*;.*/,"")
 clock_name = $2
 clock_start[clock_name]=time
 clock_pulse[clock_name] = $3 + time
 clock_period[clock_name] = $4
 clock_state[clock_name] = ($5 == "invert") ? 0 : 1
 if($5=="invert") {
  if($6!="") clock_high[clock_name]=$6
  else       clock_high[clock_name]=1
  if($7!="") clock_low[clock_name]=$7
  else       clock_low[clock_name]=0
 } else {
  if($5!="") clock_high[clock_name]=$5
  else       clock_high[clock_name]=1
  if($6!="") clock_low[clock_name]=$6
  else       clock_low[clock_name]=0
 }
 
 print "set " clock_name  " " value(clock_name, clock_state[clock_name])
 next
}

/[ \t]*stop_clock[ \t]+/{
 delete clock_start[$2]
 delete clock_pulse[$2]
 delete clock_period[$2]
 delete clock_state[$2]
 delete clock_high[$2]
 delete clock_low[$2]
 k=0; for(i in clock_start) {k=1}
 if(!k) clock_name=""
 next
}




/^[ \t]*s[ \t]+/{
 go_to_time($2+time)
 next
}

/^[ \t]*time[ \t]+/{
  go_to_time($2)
  next
}

{ print }

function go_to_time(to_time)
{

 if(clock_name)
 {
  while((a=next_event()) <= to_time)
  {
    if(a>time) print "s " a - time " ;" a
    m=split(current_clock, tmp)
    for(i=1;i<=m;i++)
    {
     clock_state[tmp[i]]=!clock_state[tmp[i]]
     print "set " tmp[i] " " value(tmp[i], clock_state[tmp[i]]) " ;" a
     time =a
    }
  }
 }
 if(to_time > time)
 {
   print "s "  to_time - time " ;" to_time
   time=to_time
 }

}

function next_event(             k, i, clock_event1, clock_event2)
{
 k=0
 for(i in clock_start)
 { 
  clock_event1 = int( (time-clock_start[i])/clock_period[i])*clock_period[i] + clock_start[i]
  while(clock_event1 <= time) {
     clock_event1+=clock_period[i]
  }
  clock_event2 = int( (time-clock_pulse[i])/clock_period[i])*clock_period[i] + clock_pulse[i]
  while(clock_event2 <= time) {
     clock_event2+=clock_period[i]
  }
  if(clock_event1 < clock_event2) 
    clock_event = clock_event1+0
  else 
    clock_event = clock_event2+0
  if(!k) { event=clock_event ; current_clock=i; k=1}
  else if(clock_event < event) 
  {
   event = clock_event 
   current_clock=i
  }
  else if( abs(clock_event - event)<1e-12 )
  {
   current_clock = current_clock " " i
  }

  print ";  time= " time  "  next event = " clock_event "  " i " event=" event
 }
 return event

}

function value(clock_name, i) {
 if(i==1) return clock_high[clock_name]
 else return clock_low[clock_name]
}

function abs(x) 
{
  return x<0? -x : x
}

