# TODO:

- [ ] SPIFFS Editor doesn't work, [issue #34](https://github.com/lbussy/keg-cop/issues/34)
- [ ] `dataHost` is locked to PIO's `./data/` interpretation
- [ ] Allow setting `dataHost` from file in application store
- [ ] Add bad update flag (i.e. if firmware failed)
- [ ] Stop all online processing if we lose WiFi
- [ ] Reduce KegScreen (not)reporting spam
- [ ] Add some form of bulk load:
    - [ ] `/api/v1/config/settings/`
    - [ ] `/api/v1/config/taps/`
- [ ] Cut down on iteration when processing posts
- [ ] Add dark mode
    - [x] Change font color on X and Y axis when using Superhero
        - [x] `index`
        - [x] `temps`
    - [x] Allow change of theme in UI
    - [x] Fix alignment (temp box and help link)
    - [ ] Add theme setting to settings storage
- [ ] Re-format the About page

# Bugs:

- [ ] Clicking on a bar (index only, temps work) always takes us to the controller's page (not live host)
- [ ] A 404 keeps the "bad" page as it's href and blows this up - top links do not work (use template or query string)
