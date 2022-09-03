# TODO:

- [ ] SPIFFS Editor doesn't work, [issue #34](https://github.com/lbussy/keg-cop/issues/34)
- [ ] Add some form of bulk load:
    - [ ] `/api/v1/config/settings/`
    - [ ] `/api/v1/config/taps/`
- [ ] Settings POST iteration
    - [ ] Cut down on iteration when processing posts
    - [ ] Make sure we send back success or fail considering all the post processing
- [ ] Re-format the About page

# Bugs:

- [ ] A 404 keeps the "bad" page as it's href and blows this up - top links do not work (use template or query string)
