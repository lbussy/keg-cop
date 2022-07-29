// Support WiFi Reset

toggleLoader("off");

function finishLoad() {
    if (!dataHostCheckDone) {
        setTimeout(populateFlow, 10);
        return;
    }
    var url = dataHost;
    while (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    // populateTempLink(); // We don't need this but it's here so I rememnber not to add it again
    // This actually resets wifi settings, after
    // this it's all over.  No need to try anything
    // else
    $.ajax({
        url: url + '/api/v1/action/wifireset/',
        type: 'PUT'
    });
}
