// Supports OTA1 page

toggleLoader("off");
var numReq = 3;
var loaded = 0;

function finishLoad() { // Get page data
    loadThisVersion(); // Populate form with controller settings
    loadThatVersion(); // Populate form with controller settings
    populateTempLink();
    pollComplete();
}

function loadThisVersion() { // Get current parameters
    var thisVersionJson = dataHost;
    if (thisVersionJson.endsWith("/")) {
        thisVersionJson = thisVersionJson.slice(0, -1)
    }
    thisVersionJson += "/api/v1/info/thisVersion/";
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
    var thatVersionJson = dataHost;
    if (thatVersionJson.endsWith("/")) {
        thatVersionJson = thatVersionJson.slice(0, -1)
    }
    thatVersionJson += "/api/v1/info/thatVersion/";
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

function finishPage() { // Display page
    // Reload here if needed
}
