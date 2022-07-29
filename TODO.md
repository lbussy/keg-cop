# TODO:

<<<<<<< HEAD
- [ ] `dataHost` is locked to PIO's `./data/` interpretation

- [ ] Tower Fan Control
    - [x] Settings JSON:
        - [x] Add tower fan enable bool
        - [x] Add tower fan setpoint long long
        - [x] Add `tfanonhigh`
    - [x] Settings UI
        - [x] Disable solenoid control settings when tower fan is enabled
        - [x] Handle data load for Settings
        - [x] Add `tfanonhigh` setting
    - [x] Enable temp control loop
    - [ ] Add a "fan on" graphic to Temps
    - [x] Add a "fan on" graphic to Index
    - [x] Do JSON sizes
    - [ ] Do documentation:
        - [ ] Kegscreen coolstate
        - [ ] Config JSON
        - [ ] Temps JSON
        - [ ] URL Report
- [ ] `coolstatetooltip` URL not re-writing
- [ ] `fanstatetooltip` URL not re-writing
- [ ] Test:
    - [ ] Kegscreen
        - [ ] Cool State Report
        - [ ] Temps Report
    - [ ] URL Report
    - [ ] Settings
        - [ ] Enable Tower Fan
        - [ ] Disable solenoid
        - [ ] Tower fan setpoint
    - [ ] UI
        - [ ] Index indicator
        - [ ] Temps indocator
=======
- [ ] Fix SPIFFS Editor
- [ ] Clicking bubblegum machine does not return to index
- [ ] Temperature data will not reload after failure on Temps
- [ ] Error Loading available version
- [ ] WiFi Reset: CORS preflight failed on: `http://kegcop.local/api/v1/action/wifireset/`
- [ ] Test
    - [ ] dataHost On
        - [ ] Dev Web Server
        - [ ] IP address
        - [ ] mDNS Name
    - [ ] dataHost Off
        - [ ] Dev Web Server
        - [ ] IP address
        - [ ] mDNS Name
>>>>>>> 489c1ab (Fix OTA (#79))
