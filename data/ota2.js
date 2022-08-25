// Support OTA2 page

// These are only used for kegcop_pre.js
var loaded = 0;
var numReq = 1;
var didClear = false; // Hold value to see if we cleared updates in doUpdates()
var didStart = false; // Hold value to see if we cleared updates in doUpdates()
var didUpdate = false;

toggleLoader("off");

function finishLoad() {
    // populateTemps(); // We don't need this but it's here so I rememnber not to add it again
    doUpdates();
}

function doUpdates() {
    if (!dataHostCheckDone) {
        setTimeout(doUpdates, 10);
        return;
    }

    // Wait for clear update
    var clearPut = window.setInterval(function () { // Poll every 5 seconds
        clearUpdate(function (semaphore) {
            didClear = semaphore;
            if (didClear == true) {
                // Cleared old updates
                clearInterval(clearPut);
            }
        });
    }, 5000);

    // Wait for update start after update clears
    var startPut = window.setInterval(function () { // Poll every 5 seconds
        if (didClear && !didStart) {
            startUpdate(function (semaphore) {
                didStart = semaphore;
                if (didStart == true) {
                    // Started updates
                    clearInterval(startPut);
                    // Start watching for controller to return
                    setTimeout(watchReset, 5000);
                }
            });
        }
    }, 5000);
}

function watchReset() {
    // Wait for clear update
    var checkUpdate = window.setInterval(function () { // Poll every 5 seconds
        clearUpdate(function (semaphore) {
            didUpdate = semaphore;
            if (didUpdate == true) {
                // Update is complete
                $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Firmware Update Complete; Redirect Pending</h4>");
                $("#message").replaceWith("<p class='card-body'>The firmware update is complete.  You will be redirected momentarily.</p>");
                setTimeout(function () { window.location.href = cleanURL("/index/"); }, 5000);
                clearInterval(checkUpdate);
            }
        });
    }, 5000);
}

function clearUpdate(callback = null) {
    var url = dataHost;
    while (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += '/api/v1/action/clearupdate/';

    $.ajax({
        method: "PUT",
        url: url
    })
        .done(function (data) {
            callback(true);
        })
        .fail(function () {
            // This will fail while controller resets
            callback(false);
        });
}

function startUpdate(callback = null) {
    var url = dataHost;
    while (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += '/api/v1/action/updatestart/';

    $.ajax({
        method: "PUT",
        url: url
    })
        .done(function (data) {
            callback(true);
        })
        .fail(function () {
            // This will fail while controller resets
            callback(false);
        });
}   
