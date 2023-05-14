# TODO:

- [ ] Consider a more generic MQTT (Like: https://github.com/lbussy/brew-bubbles/issues/50)
- [ ] Document `/fs/`
- [ ] Add configration parameter to use Telnet
- [ ] Switch to LittleFS
- [ ] Make a config/flowmeter backup

Crash (this might be the one):

```
2023-04-26T18:07:37Z V: KegScreen reporting not enabled, skipping (Tap Info).
2023-04-26T18:07:50Z N: Tap 0 is kicked.
2023-04-26T18:07:50Z V: 2023-04-26T18:07:50Z Flowmeter Config Save: Saving configuratV: ion.
KegScreen reporting not enabled, skipping (Kick Report).
Guru Meditation Error: Core  0 panic'ed (Unhandled debug exception). 
Debug exception reason: Stack canary watchpoint triggered (esp_timer) 
Core  0 register dump:
PC      : 0x400dfd22  PS      : 0x00060a36  A0      : 0x800e1d12  A1      : 0x3ffbabe0  
A2      : 0x3ffc4f98  A3      : 0x3ffbbc88  A4      : 0x3ffbb0c0  A5      : 0x3ffbac24  
A6      : 0x00000001  A7      : 0x3ffbbc88  A8      : 0x00000001  A9      : 0x3ffbab70  
A10     : 0x3f40351c  A11     : 0x00000001  A12     : 0x000000ff  A13     : 0x0000ff00  
A14     : 0x00ff0000  A15     : 0xff000000  SAR     : 0x00000017  EXCCAUSE: 0x00000001  
EXCVADDR: 0x00000000  LBEG    : 0x4008b62d  LEND    : 0x4008b63d  LCOUNT  : 0xfffffffc  

Backtrace: 0x400dfd1f:0x3ffbabe0 0x400e1d0f:0x3ffbaf90 0x400e46e2:0x3ffbb0a0 0x400e4755:0x3ffbbd10 0x400e4bfb:0x3ffbbd50 0x4016687f:0x3ffbbd70

  #0  0x400dfd1f:0x3ffbabe0 in ArduinoJson6194_F1::VariantRef ArduinoJson6194_F1::ObjectRef::getOrAddMember<char const>(char const*) const at .pio/libdeps/lolin_d32/ArduinoJson/src/ArduinoJson/Object/ObjectRef.hpp:210
      (inlined by) ArduinoJson6194_F1::MemberProxy<ArduinoJson6194_F1::ObjectRef, char const*>::getOrAddUpstreamMember() const at .pio/libdeps/lolin_d32/ArduinoJson/src/ArduinoJson/Object/MemberProxy.hpp:191
      (inlined by) ArduinoJson6194_F1::enable_if<!ArduinoJson6194_F1::is_array<int>::value, ArduinoJson6194_F1::MemberProxy<ArduinoJson6194_F1::ObjectRef, char const*>&>::type ArduinoJson6194_F1::MemberProxy<ArduinoJson6194_F1::ObjectRef, char const*>::operator=<int>(int const&) at .pio/libdeps/lolin_d32/ArduinoJson/src/ArduinoJson/Object/MemberProxy.hpp:49
      (inlined by) Taps::save(ArduinoJson6194_F1::ObjectRef) const at src/flowconfig.cpp:143
  #1  0x400e1d0f:0x3ffbaf90 in Flowmeter::save(ArduinoJson6194_F1::ObjectRef) const at src/flowconfig.cpp:267
  #2  0x400e46e2:0x3ffbb0a0 in serializeFlowConfig(Print&) at src/flowconfig.cpp:133
  #3  0x400e4755:0x3ffbbd10 in saveFlowConfig() at src/flowconfig.cpp:85
  #4  0x400e4bfb:0x3ffbbd50 in logFlow() at src/flowmeter.cpp:127
  #5  0x4016687f:0x3ffbbd70 in timer_process_alarm at /Users/ficeto/Desktop/ESP32/ESP32S2/esp-idf-public/components/esp_timer/src/esp_timer.c:360
      (inlined by) timer_task at /Users/ficeto/Desktop/ESP32/ESP32S2/esp-idf-public/components/esp_timer/src/esp_timer.c:386

ELF file SHA256: a595e9a8a5537282

Rebooting...
```