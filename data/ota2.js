// Support OTA2 page

var loaded = 0;
var numReq = 1;

toggleLoader("off");

function finishLoad() { // Load the page's JS elements
    // populateTemps(); // We don't need this but it's here so I rememnber not to add it again
    if (!dataHostCheckDone) {
        setTimeout(finishLoad, 10);
        return;
    }

    var url = dataHost;
    while (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    var urlClear = url + '/api/v1/action/clearupdate/';
    var urlStart = url + '/api/v1/action/updatestart/';

    $.ajax({ // Clear any previous update flags
        url: urlClear,
        type: 'PUT'
    });

    $.ajax({ // Semaphore the controller that it's safe to proceed
        url: urlStart,
        type: 'PUT'
    });

    // TODO:  Test OTA process
    setTimeout(setupWatchReset, 5000);
}

function setupWatchReset() {
    dataHostCheckDone = false;
    checkDataHost();
    watchReset();
}

function watchReset() {
    var running = false;

    if (!dataHostCheckDone) {
        setTimeout(finishLoad, 10);
        return;
    }

    var url = dataHost;
    while (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += '/api/v1/action/clearupdate/';

    // Wait for update to complete
    var intervalID = window.setInterval(function () { // Poll every 5 seconds
        checkSemaphore(function (semaphore) {
            didupdate = semaphore;
            if ((didupdate == true) && (running == false)) {
                running = true;
                // Update is complete
                $.ajax({
                    url: url,
                    type: 'PUT'
                });
                $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Firmware Update Complete; Redirect Pending</h4>");
                $("#message").replaceWith("<p class='card-body'>The firmware update is complete.  You will be redirected momentarily.</p>");
                setTimeout(function () { window.location.href = cleanURL("/index/"); }, 5000);
            }
        });
    }, 10000);
}
