# TODO:

## Testing

- ???

## Features

- WiFi Reset does not work properly (notes below)
- Allow update of firmware to re-apply flow.json
- Add target pushes
- Figure out what the sendUpdateCount() report is (RandR+)
- Handle Imperial/metric
    - Display (web)
    - Settings post
- Filter small pours

## Bugs

- ???

# Complete

- ~~Add web pages~~
- ~~Add config~~
- ~~Add wifi~~
- ~~Add mDNS~~
- ~~Add ntp~~
- ~~Add serial~~
- ~~Add web server~~
- ~~Add thisVersion~~
- ~~Add temp sensors~~
- ~~Add flow meters~~
- ~~Add thermostat~~
- ~~Add thatVersion~~
- ~~Add OTA~~
- ~~Design Index~~
- ~~Add web page scripting~~
    - ~~Ota~~
    - ~~Ota2~~
    - ~~Reset~~
    - ~~Wifi~~
    - ~~Wifi2~~
    - ~~About~~
    - ~~Index~~
    - ~~OnLoad spinner~~
    - ~~Settings~~
- ~~Design Settings~~
- ~~Decide on active/inactive vs numtaps~~
- ~~Allow graphics to load on controller reset~~
- ~~Fix flow.json defaults - All blanks on initial start, missing data subsequently~~
- ~~Add temp sensor traces to Tallboy and regular board (check others)~~
- ~~Fix alignment on Tap 0~~
- ~~Add RPints compatibility switches~~
- ~~Add solenoid status/use~~
- ~~Add serial configuration for RPints~~
- ~~Add RPints report~~
- ~~Add a defer on reporting while pulling a pour~~
- ~~Add a kick trigger~~
- ~~Add POST handlers~~
- ~~Change max on key progress bars to match settings~~
- ~~Add demo tap list~~
- ~~Round display of percentage on progress bar~~
- ~~Add bulk POST handler~~
- ~~Display remaining in glasses? (pints or 0.5l)~~

### Notes

```
2020-05-19T01:04:35Z V: Serving /config/.
[E][vfs_api.cpp:27] open(): 2/wifi.htm does not start with /
[E][vfs_api.cpp:27] open(): 2/wifi.htm.gz does not start with /
2020-05-19T01:05:43Z V: Processing /wifi2/.
*WM: [1] resetSettings 
*WM: [3] WiFi station enable. 
*WM: [1] Settings erased. 
E (253106) task_wdt: Task watchdog got triggered. The following tasks did not reset the watchdog in time:
E (253106) task_wdt:  - async_tcp (CPU 1)
E (253106) task_wdt: Tasks currently running:
E (253106) task_wdt: CPU 0: IDLE0
E (253106) task_wdt: CPU 1: loopTask
E (253106) task_wdt: Aborting.
abort() was called at PC 0x40106668 on core 0

Backtrace: 0x4008c644:0x3ffbe170 0x4008c875:0x3ffbe190 0x40106668:0x3ffbe1b0 0x400847f5:0x3ffbe1d0 0x4018b28f:0x3ffbc160 0x40107bc6:0x3ffbc180 0x4008a3e5:0x3ffbc1a0 0x40088c01:0x3ffbc1c0

Rebooting...
```
