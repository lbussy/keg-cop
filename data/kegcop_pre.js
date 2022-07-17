// Common file/functions for all Keg Cop pages

var dataHost = "";

// Detect unloading state during getJSON
var unloadingState = false;
window.addEventListener('beforeunload', function (event) {
    unloadingState = true;
});

function preLoad() {
    // Check for devServer setup

    // To set data server, use the following syntax in console:
    //
    // >>   localStorage.setItem("dataHost", "http://kegcop.local/");
    //
    // Note that the full URL including 'http:// and
    // trailing '/' is required
    dataHost = localStorage.getItem("dataHost") || "";
    // Also remember that this must be cleared for things to work
    // normally again:
    //
    // >>   localStorage.setItem("dataHost", "http://kegcop.local/");

    // Make sure the page is 100% loaded
    if (document.readyState === "ready" || document.readyState === "complete") {
        startLoad();
    } else {
        document.onreadystatechange = function () {
            startLoad();
        };
    }
}

function startLoad() {
    $(document).tooltip({ // Enable tooltips
        'selector': '[data-toggle=tooltip]',
        'placement': 'left',
        'toggleEnabled': true
    });
    // Call finishLoad() if it exists (page handler)
    if (typeof finishLoad === "function") {
        finishLoad();
    };
}

function populateTemps(callback = null) {
    // Only show "Temperatures" tab if we have sensors
    var url = dataHost + "api/v1/info/sensors";
    var config = $.getJSON(url, function () {
    })
        .done(function (temps) {
            try {
                if (temps.displayenabled == true || dataHost) {
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

function toggleLoader(status) {
    // Supports page load spinner
    var loader = document.getElementById("loader");
    var tempsApp = document.getElementById("tempsApp");
    var settingsApp = document.getElementById("settingsApp");
    var indexApp = document.getElementById("indexApp");
    if (status === "on") {
        if (loader) {
            loader.style.display = "block";
        }
        if (tempsApp) {
            tempsApp.style.visibility = "hidden";
        }
        if (settingsApp) {
            settingsApp.style.visibility = "hidden";
        }
        if (indexApp) {
            indexApp.style.visibility = "hidden";
        }
    } else {
        if (loader) {
            loader.style.display = "none";
        }
        if (tempsApp) {
            tempsApp.style.visibility = "visible";
        }
        if (settingsApp) {
            settingsApp.style.visibility = "visible";
        }
        if (indexApp) {
            indexApp.style.visibility = "visible";
        }
    }
}

function pollComplete() {
    if (loaded == numReq) {
        finishPage();
    } else {
        setTimeout(pollComplete, 300); // try again in 300 milliseconds
    }
}
