// Supports Controller Reset page

toggleLoader("off");

// Pre Loader Variables
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
    url += "/api/v1/action/reset/";

    $.ajax({
        url: url,
        headers: {"X-KegCop-Secret": secret },
        type: 'PUT'
    })
        .done(function () {
            $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Controller Reset Complete</h4>");
            $("#message").replaceWith("<p class='card-body'>Your Keg Cop's controller has been reset and will return momentarily. You will be redirected to the home page when that is complete.</p>");
            setTimeout(setupWatchReset, 5000);
        })
        .fail(function () {
            $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Controller Reset Failed; Redirect Pending</h4>");
            $("#message").replaceWith("<p class='card-body'>The controller reset failed. You will be redirected momentarily.</p>");
            setTimeout(function () { window.location.href = cleanURL("/index/"); }, 5000);
        })
        .always(function () {
            //
        });
}

function setupWatchReset() {
    dataHostCheckDone = false;
    checkDataHost();
    watchReset();
}

function watchReset() {
    // Wait for restart to complete
    if (!dataHostCheckDone) {
        setTimeout(watchReset, 10);
        return;
    }

    var intervalID = window.setInterval(function () { // Poll every 5 seconds
        checkSemaphore(function (semaphore) {
            didreset = semaphore;
            if (didreset == true) {
                // Reset is complete
                window.clearInterval(intervalID);
                $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Controller Reset Complete; Redirect Pending</h4>");
                $("#message").replaceWith("<p class='card-body'>The controller reset is complete. You will be redirected momentarily.</p>");
                setTimeout(function () { window.location.href = cleanURL("/index/"); }, 5000);
            }
        });
    }, 5000);
}
