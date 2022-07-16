// Common file/functions for all Keg Cop pages

var thisHost = "";

// Detect unloading state during getJSON
var unloadingState = false;
$(window).bind("beforeunload", function () {
    unloadingState = true;
});

// Turn off tooltips on radio button change {" "}
$('input[type=radio]').change(function () {
    $('[data-toggle="tooltip"], .tooltip').tooltip("hide");
});

// Attach the event after the page loads
if (window.addEventListener) window.addEventListener("load", preLoad, false);
else if (window.attachEvent) window.attachEvent("onload", preLoad);
else window.onload = preLoad;

function preLoad() {
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

const loadScript = (FILE_URL, async = true, type = "text/javascript") => {
    // Load a supporting file as a Promise event
    return new Promise((resolve, reject) => {
        try {
            const scriptEle = document.createElement("script");
            scriptEle.type = type;
            scriptEle.async = async;
            scriptEle.src = FILE_URL;

            scriptEle.addEventListener("load", (ev) => {
                resolve({ status: true });
            });

            scriptEle.addEventListener("error", (ev) => {
                reject({
                    status: false,
                    message: `Failed to load script: ${FILE_URL}.`
                });
            });

            document.body.appendChild(scriptEle);
        } catch (error) {
            reject(error);
        }
    });
};

loadScript("devServer.js")
    .then(data => {
        if (!thishost) {
            console.log("Dev server script loaded successfully but 'devScript' was missing.", data);
        } else {
            console.log("Dev server script loaded successfully.", data);
        }
    })
    .catch(err => {
        console.log("Failed to load devScript.");
        console.error(err);
    });

function populateTemps(callback = null) {
    // Only show "Temperatures" tab if we have sensors
    var url = thisHost + "api/v1/info/sensors";
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

// AJAX errors
// Used in index, settings, temps

tempAlert = function () { }
tempAlert.warning = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-warning" id="warnTempError">'
        _div += '<button type="button" class="close" data-dismiss="alert">&times;</button><p class="mb-0">'
        _div += '<strong>Temps Warning: </strong><span>' + message + '</span></p></div>'
    }
    $('#flowAlert_placeholder').html(_div);
}

configAlert = function () { }
configAlert.warning = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-warning" id="warnConfigError">'
        _div += '<button type="button" class="close" data-dismiss="alert">&times;</button><p class="mb-0">'
        _div += '<strong>Config Warning: </strong><span>' + message + '</span></p></div>'
    }
    $('#flowAlert_placeholder').html(_div);
}

flowAlert = function () { }
flowAlert.warning = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-warning" id="warnFlowError">'
        _div += '<button type="button" class="close" data-dismiss="alert">&times;</button><p class="mb-0">'
        _div += '<strong>Flow Warning: </strong><span>' + message + '</span></p></div>'
    }
    $('#flowAlert_placeholder').html(_div);
}

settingsAlert = function () { }
settingsAlert.error = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-danger" id="warnSettingsError">'
        _div += '<button type="button" class="close" data-dismiss="alert">&times;</button><p class="mb-0">'
        _div += '<strong>Settings Error: </strong><span>' + message + '</span></p></div>'
    }
    $('#settingsAlert_placeholder').html(_div);
}
