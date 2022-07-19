// Supports Controller Reset page

toggleLoader("off");
var okToReset = false;

function finishLoad() {
    // Catch event from kegcop_pre.js
    populateTemps();
    doResetSignal();
}

function doResetSignal() {
    $.ajax({
        url: '/api/v1/action/reset/',
        type: 'put',
        success: function () {
            myVar = setTimeout(watchReset, 5000);
        },
        fail: function () {
            $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Controller Reset Failed; Redirect Pending</h4>");
            $("#message").replaceWith("<p class='card-body'>The controller reset failed. You will be redirected momentarily.</p>");
            setTimeout(function () { window.location.href = "index"; }, 5000);
        }
    });
}

function watchReset() {
    // Wait for restart to complete
    var intervalID = window.setInterval(function () { // Poll every 5 seconds
        checkSemaphore(function (semaphore) {
            didreset = semaphore;
            if (didreset == true) {
                // Reset is complete
                window.clearInterval(intervalID);
                $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Controller Reset Complete; Redirect Pending</h4>");
                $("#message").replaceWith("<p class='card-body'>The controller reset is complete. You will be redirected momentarily.</p>");
                setTimeout(function () { window.location.href = "index"; }, 5000);
            }
        });
    }, 5000);
}

function checkSemaphore(callback) { // Check to see if the update is complete
    var url = dataHost;
    if (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/action/ping/";
    var jqxhr = $.get(url)
        .done(function (data) {
            callback(true);
        })
        .fail(function () {
            // This will fail while controller resets
            callback(false);
        });
}