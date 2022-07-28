// Support WiFi Reset

toggleLoader("off");
checkDataHost();    // Check if we are using a dataHost

function finishLoad() {
    if (!dataHostCheckDone) {
        setTimeout(finishLoad, 10);
        return;
    }
    // populateTempLink(); // We don't need this but it's here so I rememnber not to add it again
    // This actually resets wifi settings, after
    // this it's all over.  No need to try anything
    // else
    $.ajax({
        url: '/api/v1/action/wifireset/',
        type: 'PUT'
    });
}
