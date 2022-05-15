/*

Case One  / Case Two
2 vars    5 vars
2 vars    3 vars
          2 vars

"head,var1,var2,...,vanN"

set_type = head character;

s - set val                  c - clock
o - state of case one        i - interval
                             k - state of case two

s = 115                     c = 099
o = 111                     i = 105
                            k = 107

[c,hour,minute,dur1,rep,is_clock_set,]
[i,dur2,pause,is_inter_set,]
[k,state,state_set,]

[s,system_set_val,is_system_set,]
[o,state,state_set,]


| Temperautre | Humidity | Carbon | Water Temperature | Water Level | Lighting | Watering |
       0           1          2             3                 4           5         6

CaseOne

  int system_val; // 0

  int system_set_val; // 1

  bool system_state; // 2

  bool is_system_set; // 3
  bool is_state_set; // 4

  uint8_t system_num; // 5




CaseTwo

  int system_val; // 0

  int system_time_h;  // 1
  int system_time_m; // 2
  int system_dur1; // 3
  bool system_rep; // 4

  int system_dur2; // 5
  int system_pause; // 6

  bool system_state; // 7

  bool is_clock_set; // 8
  bool is_inter_set; // 9
  bool is_state_set; // 10

  uint8_t system_num; // 11



   [0-6(7)]          [0-2(3)]
systems_pointer / levels_pointer / places_pointer


 DISPLAY LAYOUT

| |0| | | | | | | | | | | | | | | | | | |
|1| | | | | | | | | |2| | | | | | | | | |
|3| | | | | | | | | | | | | | | | | | | |
| | | | | | | | | | | | | | | | | | | | |
      r  c
 0 - {0, 1}
 1 - {1, 0}
 2 - {1, 11}
 3 - {2, 0}

                [0-19] [0-3]
API = lcd.print(column, row)


 *** *** COMMANDS TO CONTROL RELAYS *** ***

T - toggle Temperature
  a - turn on Temperature
  b - turn off Temperaure (!)

H - toggle Humidity
  g - turn on Humidity
  h - turn off Humidity (!)

C - toggle Carbon
  i - turn on Carbon
  j - turn off Carbon (!)

X - toggle Water Temperature
  k - turn on Water Temperature
  l -  turn off Water Temperaure (!)

Y - toggle Water Level
  m - turn on Water Level
  n - turn off Water Level (!)

L - toggle Light
  c - turn on Light
  d - turn off Light (!)

W - toggle Water
  o - turn on Water
  p - turn off Water (!)



F - toggle Fan (not deployed)
  e - turn on Fan
  f - turn off Fan


DateTimeStamp = "yyyy-mm-dd hh:mm"


 *** Unused yet ***
button_state_class.old_variable_name
card_state_class.old_variable_name
card_text_class.old_variable_name


*/

