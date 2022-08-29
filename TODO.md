# TODO:

- [ ] `dataHost` is locked to PIO's `./data/` interpretation
- [ ] Allow setting `dataHost` from file in application store
- [ [ Add KegScreeen:
    - [x] Service
    - [ ] REST API
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
    - [ ] Allow change of theme in UI
    - [x] Fix alignment (temp box and help link)

# Bugs:

- [ ] Clicking on a bar (index only, temps work) always takes us to the controller's page (not live host)
- [ ] A 404 keeps the "bad" page as it's href and blows this up - top links do not work
