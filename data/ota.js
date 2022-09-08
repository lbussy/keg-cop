// Support OTA2 page

toggleLoader("off");

// Pre Loader Variables
var loaded = 0;
var numReq = 1;
// Page Variables
var didClear = false; // Hold value to see if we cleared updates in doUpdates()
var didStart = false; // Hold value to see if we cleared updates in doUpdates()
var didUpdate = false;
// Semaphores
var clearUpdateRunning = false;

function finishLoad() {
    // chooseTempMenu();// We don't need this but it's here so I rememnber not to add it again
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
    while (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += '/api/v1/action/clearupdate/';

    if (clearUpdateRunning) return;
    clearUpdateRunning = true;
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
        })
        .fail(function (data) {
            clearUpdateRunning = false;
        });
}

function startUpdate(callback = null) {
    var url = dataHost;
    while (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += '/api/v1/action/updatestart/';

    $.ajax({
        method: "PUT",
        data: { secret: secret },
        url: url
    })
        .done(function (data) {
            $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Application Update Executing</h4>");
            var message = "<p class='card-body'>Your Keg Cop's firmware is being updated to the latest version. This can take up to 5 minutes, during which your Keg Cop will be unresponsive. Please do not disconnect the power or reset your Keg Cop while this process is taking place.";
            message += "<br /><br />If you wish to observe your controller's LED during the process, the LED will flash as the update is in progress.";
            message += "<br /><br />When the update is complete, the controller will re-load your original application settings. If this step fails, you will need to manually reconfigure all application settings. WiFi settings will not be affected.";
            message += "<br /><br />Be sure to check the update page again when finished to see if the versions you expect are present on both the Firmware and the Filesystem.  If the versions are not what you expect, there is no harm in performing the actions again.</p>";
            message += "<p class='card-body text-danger' style='display: inline-block;'><strong>Do not refresh this page. If you do, you will lose your settings and not be able to track the upgrade process.</strong></p>";
            $("#message").replaceWith(message);
            callback(true);
        })
        .fail(function () {
            // This will fail while controller resets
            $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Application Update Failed; Redirect Pending</h4>");
            $("#message").replaceWith("<p class='card-body'>The application update failed. You will be redirected momentarily. Additional details may be found in the application update tab.</p>");
            setTimeout(function () { window.location.href = cleanURL("/index/"); }, 5000);
        });
}   
