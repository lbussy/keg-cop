# TODO:

- [ ] SPIFFS Editor doesn't work, [issue #34](https://github.com/lbussy/keg-cop/issues/34)
- [ ] `dataHost` is locked to PIO's `./data/` interpretation
- [ ] Allow setting `dataHost` from file in application store
- [ ] Add some form of bulk load:
    - [ ] `/api/v1/config/settings/`
    - [ ] `/api/v1/config/taps/`
- [ ] Cut down on iteration when processing posts
- [ ] Make sure we send back success or fail considering all the post processing
- [ ] Re-format the About page

# Bugs:

- [ ] Clicking on a bar (index only, temps work) always takes us to the controller's page (not live host)
- [ ] A 404 keeps the "bad" page as it's href and blows this up - top links do not work (use template or query string)
