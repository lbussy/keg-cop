# TODO:

- [ ] `dataHost` is locked to PIO's `./data/` interpretation
- [ [ Add KegScreeen:
    - [x] Service
    - [ ] REST API
- [x] Add `secret` for impactful posts
    - [x] Calibrate `settings.js`
    - [x] Reset Controller `controllerreset.js`
    - [x] Update Application `ota2.js`
    - [x] Reset WiFi `wifireset.js`
- [ ] Add bad update flag (i.e. if firmware failed)
- [ ] Kill DRD if:
    - [ ] We fail to connect to WiFi and restart
    - [ ] We failed to get a time hack and restart
- [ ] Stop all online processing if we lose WiFi
- [ ] Reduce KegScreen (not)reporting spam
- [ ] Add some form of bulk load:
    - [ ] `/api/v1/config/settings/`
    - [ ] `/api/v1/config/taps/`
