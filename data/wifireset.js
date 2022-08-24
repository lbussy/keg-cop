// Support WiFi Reset

var loaded = 0;
var numReq = 1;

toggleLoader("off");

function finishLoad() {
    if (!dataHostCheckDone) {
        setTimeout(finishLoad, 10);
        return;
    }

    var url = dataHost;
    while (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += '/api/v1/action/wifireset/'

    // chooseTempMenu(); // We don't need this but it's here so I rememnber not to add it again
    // This actually resets wifi settings, after
    // this it's all over.  No need to try anything
    // else
    $.ajax({
        url: url,
        type: 'PUT'
    });
}
