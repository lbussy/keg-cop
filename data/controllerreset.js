// Supports Controller Reset page

toggleLoader("off");
var okToReset = false;
var loaded; // Just a placeholder here
var numReq; // Just a placeholder here

function finishLoad() {
    // Catch event from kegcop_pre.js
    chooseTempMenu();
    doResetSignal();
}

function doResetSignal() {
    var url = dataHost;
    if (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/action/reset/";
    $.ajax({
        url: url,
        type: 'PUT',
        data: {},
        success: function () {
            $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Controller Reset Complete</h4>");
            $("#message").replaceWith("<p class='card-body'>Your Keg Cop's controller has been reset and will return momentarily. You will be redirected to the home page when that is complete.</p>");
            setTimeout(watchReset, 5000);
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
                var link = document.getElementById("home");
                setTimeout(function () { link.click(); }, 5000);
            }
        });
    }, 5000);
}
