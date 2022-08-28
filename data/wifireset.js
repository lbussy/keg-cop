// Supports Controller Reset page

toggleLoader("off");``
var loaded = 0;
var numReq = 0 + numReqPre; // Just a placeholder here

function finishLoad() {
    // Catch event from kegcop_pre.js
    chooseTempMenu();
    pollComplete();
}

function finishPage() {
    doResetSignal();
}

function doResetSignal() {
    if (!dataHostCheckDone) {
        setTimeout(doResetSignal, 10);
        return;
    }

    var url = dataHost;
    while (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/action/wifireset/";

    $.ajax({
        url: url,
        data: { secret: secret },
        type: 'PUT'
    })
        .done(function () {
            $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Controller Reset Complete</h4>");
            $("#message").replaceWith("<p class='card-body'>Your Keg Cop's WiFi credentials have been reset and you are disconnected from the network and application. You will need to configure the WiFi settings again from the captive portal.<br /><br />You may close this page.</p>");
        })
        .fail(function () {
            $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>WiFi Reset Failed; Redirect Pending</h4>");
            $("#message").replaceWith("<p class='card-body'>The WiFi reset failed. You will be redirected momentarily.</p>");
            setTimeout(function () { window.location.href = cleanURL("/index/"); }, 5000);
        });
}
