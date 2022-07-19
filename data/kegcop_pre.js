// Common file/functions for all Keg Cop pages

var dataHost = "";
var useTemps = false;

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
    //
    dataHost = localStorage.getItem("dataHost") || "";
    //
    // Also remember that this must be cleared for things to work
    // normally again:
    //
    // >>   localStorage.setItem("dataHost", "http://kegcop.local/");
    //

    // To set temperature link display, use the following syntax in console:
    //
    // >>   localStorage.setItem("useTemps", true);
    //
    useTemps =  (JSON.parse(localStorage.getItem("useTemps")) === true);
    // (We have to serialize the vlaue of lical storage to get a bool)
    //
    // Also remember that this must be set to false for things to work
    // normally again:
    //
    // >>   localStorage.setItem("useTemps", false);
    //

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
        //'placement': 'left',
        'toggleEnabled': true
    });
    // Call finishLoad() if it exists (page handler)
    if (typeof finishLoad === "function") {
        finishLoad();
    };
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

async function chooseTempMenu(callback = null) {
    var url = dataHost;
    if (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/info/tempcontrol/";
    try {
        const response = await fetch(url);
        // response.status holds http code

        if (response.ok === true || useTemps === true) {
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
    } catch (err) {
        // Can check for (err) here
    }
    if (typeof callback == "function") {
        callback();
    }
}
