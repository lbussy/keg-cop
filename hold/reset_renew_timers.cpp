    // // mDNS Reset Timer - Helps avoid the host not found issues
    // Ticker mDNSTimer;
    // mDNSTimer.attach(MDNSTIMER, mdnsreset);

    // // Reboot timer - I wish controllers could be counted on to be more
    // // stable but at least it only takes a few seconds.
    // Ticker rebootTimer;
    // rebootTimer.attach(REBOOTTIMER, [](){
    //     Log.notice(F("Reboot timer - rebooting system." CR));
    //     // All the machinations are necessary or else restart() hangs
    //     portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    //     vTaskEnterCritical(&mux);
    //     ESP.restart();
    //     vTaskExitCritical(&mux);
    // });