# TODO:

## Testing

- Test all settings flow/application

## Features

- Add target pushes
- Add links to context help
- Move JS to separate files
- Don't let a kick disable a tap in RPints compat

## Bugs

- ??

## Before Release

- Remove /edit/
- Update tag
- Update website

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
- ~~WiFi Reset does not work properly~~
- ~~Allow update of firmware to re-apply flow.json~~
- ~~Handle Imperial/metric~~
    - ~~Display (web)~~
      - ~~Index~~
      - ~~Settings~~
    - ~~Settings posting~~
    - ~~Thermostat~~
- ~~Add temp control status to web UI~~
- ~~Add temps display to web UI~~
- ~~sendUpdateCount() report is sent when sendPulseCount() is not warranted (RandR+)~~
- ~~Update page data on timer~~
  - ~~Temps~~
  - ~~Flow~~
  - ~~Y-Axis scale~~
- ~~Wifi credentials get erased on reset~~
- ~~Sometimes will not connect after setting credentials~~
- ~~Make page load spinner work on Temps and Index~~
- ~~Add hotlink back to main page on temps~~
- ~~Add cooling indicator to temp display~~
- ~~Figure out Tstat logic~~
- ~~Figure out what to do with tstat.control (if anything)~~
- ~~Add units display to PPU (and maybe others)~~
- ~~See if new POST processor can handle larger form submits (less buttons)~~
  - ~~Taps~~
    - ~~Tap 1~~
    - ~~Tap 2~~
    - ~~Tap 3~~
    - ~~Tap 4~~
    - ~~Tap 5~~
    - ~~Tap 6~~
    - ~~Tap 7~~
    - ~~Tap 8~~
  - ~~Controller~~
  - ~~Temperature~~
    - ~~Control~~
    - ~~Sensors~~
- ~~Look into why solenoid chatters on flash (On, Off, On) (from controller resetting due to PIO terminal)~~
- ~~Deal with disconnected sensors~~
  - ~~Allow enable/disable sensors~~
  - ~~Remove the sensor display from the main page if the selected control point is disabled~~
  - ~~Web display~~
  - ~~Control point~~
- ~~Make clicks on temps go to settings for that sensor~~
- ~~Don't show disabled sensors in temps (/temperatures/)~~
- ~~Filter small pours~~
- ~~Create single-form submits for Targets~~
  - ~~URL~~
  - ~~Cloud~~
- ~~Re-do justification on Temperature Control page~~
- ~~Refresh flow settings on timer~~
- ~~When POST is blocked by form, buttons are disabled~~
- ~~Seeem to be losing first tap on each flash occasionally~~
- ~~Examine hostname test, try to reload without a page reload~~
- ~~On hostname change, make page refresh to new name~~
- ~~Allow accepting IP or not local in settings~~
- ~~Update remaining after tap pull?  If not, remove refresh timer~~
- ~~Changing metric/imperial, reload settings?~~
- ~~Shorten up switch in settings.htm with tapNum~~
