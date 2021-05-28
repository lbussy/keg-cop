// Supports OTA1 page

var unloadingState = false;
var numReq = 3;
var loaded = 0;

// Detect unloading state during getJSON
$(window).bind("beforeunload", function () {
    unloadingState = true;
});

function populatePage() { // Get page data
    loadThisVersion(); // Populate form with controller settings
    loadThatVersion(); // Populate form with controller settings
    populateTemps();
    pollComplete();
}

function loadThisVersion() { // Get current parameters
    var thisVersionJson = "/api/v1/info/thisVersion/";
    var thisVersion = $.getJSON(thisVersionJson, function () {
    })
        .done(function (thisVersion) {
            try {
                $('#thisFWVersion').text(thisVersion.fw_version);
                $('#thisFSVersion').text(thisVersion.fs_version);
            }
            catch {
                $('#thisFWVersion').text("Error loading.");
                $('#thisFSVersion').text("Error loading.");
            }
        })
        .fail(function () {
            $('#thisFWVersion').text("Error loading.");
            $('#thisFSVersion').text("Error loading.");
        })
        .always(function () {
            // Can post-process here
        });
}

function loadThatVersion() { // Get current parameters
    var thatVersionJson = "/api/v1/info/thatVersion/";
    var thatVersion = $.getJSON(thatVersionJson, function () {
    })
        .done(function (thatVersion) {
            try {
                $('#thatFWVersion').text(thatVersion.fw_version);
                $('#thatFSVersion').text(thatVersion.fs_version);
                document.getElementById("proceed").disabled = false;
            }
            catch {
                $('#thatFWVersion').text("Error loading.");
                $('#thatFSVersion').text("Error loading.");
            }
        })
        .fail(function () {
            $('#thatFWVersion').text("Error loading.");
            $('#thatFSVersion').text("Error loading.");
        })
        .always(function () {
            // Can post-process here
        });
}

function populateTemps(callback = null) { // Get configuration settings
    var url = "/api/v1/info/sensors/";
    var config = $.getJSON(url, function () {
    })
        .done(function (temps) {
            try {
                if (temps.displayenabled == true) {
                    if (!$('#displaytemplink').is(':visible')) {
                        $('#displaytemplink').toggle();
                    }
                } else {
                    if ($('#displaytemplink').is(':visible')) {
                        $('#displaytemplink').toggle();
                    }
                }

                if (loaded < numReq) {
                    loaded++;
                }
                if (typeof callback == "function") {
                    callback();
                }
            }
            catch {
                if (!unloadingState) {
                    // No need to handle an error here since this simply sets up the menu
                }
            }
        })
        .fail(function () {
            if (!unloadingState) {
                // No need to handle an error here since this simply sets up the menu
            }
        })
        .always(function () {
            // Can post-process here
        });
}

function pollComplete() {
    if (loaded == numReq) {
        finishPage();
    } else {
        setTimeout(pollComplete, 300); // try again in 300 milliseconds
    }
}

function finishPage() { // Display page
    // Reload here if needed
}
