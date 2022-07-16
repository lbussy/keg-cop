// Common file for all Keg Cop pages

var thisHost = "";
var unloadingState = false;

// Detect unloading state during getJSON
$(window).bind("beforeunload", function () {
    unloadingState = true;
});

// Turn off tooltips on radio button change {" "}
$('input[type=radio]').change(function () {
    $('[data-toggle="tooltip"], .tooltip').tooltip("hide");
});

// Attach the event after the page loads
if (window.addEventListener) window.addEventListener("load", startLoad, false);
else if (window.attachEvent) window.attachEvent("onload", startLoad);
else window.onload = startLoad;

function startLoad() {
    // Make sure the page is 100% loaded
    if (document.readyState === "ready" || document.readyState === "complete") {
        allOnLoad();
    } else {
        document.onreadystatechange = function () {
            allOnLoad();
        };
    }
}

function allOnLoad() {
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
