// Common file/functions for all Keg Cop pages

var dataHost = "";
var usingDataHost = false;
var dataHostCheckDone = false;
var useTemps = false;

// Use this here to enforce running first
dataHost = localStorage.getItem("dataHost");
if (dataHost && !dataHost.endsWith("/")) dataHost += "/";

getUseTemps();

// Attach the event after the page loads
if (window.addEventListener) window.addEventListener("load", preLoad, false);
else if (window.attachEvent) window.attachEvent("onload", preLoad);
else window.onload = preLoad;

// Detect unloading state during getJSON
var unloadingState = false;
window.addEventListener("beforeunload", function (event) {
    unloadingState = true;
});

window.onclick = function (event) {
    var type = event.target.type;
    // Skip random clicks, form items and context help
    if (typeof type === 'undefined' || type == "submit" || type == "radio" || event.target.id == "noChange") {
        return;
    }
    event.preventDefault();
    const newURL = cleanURL(getEventTarget(event));
    if (newURL) {
        window.open(newURL, "_self");
        return false;
    }
    return;
}

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
    fastTempsMenu();
    $(document).tooltip({ // Enable tooltips
        'selector': '[data-toggle=tooltip]',
        //'placement': 'left',
        'toggleEnabled': true
    });
    // Call finishLoad() if it exists (page handler)
    if (typeof finishLoad === "function") {
        checkDataHost();    // Check if we are using a dataHost
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

function fastTempsMenu() {
    // Fast show/don't show
    if (JSON.parse(sessionStorage.getItem("useTemps")) === true) {
        if (!$('#displaytemplink').is(':visible')) {
            $('#displaytemplink').toggle();
        }
    } else {
        if ($('#displaytemplink').is(':visible')) {
            $('#displaytemplink').toggle();
        }
    }
}

async function chooseTempMenu(callback = null) {
    if (!dataHostCheckDone) {
        setTimeout(chooseTempMenu, 10);
        return;
    }

    var url = dataHost;
    if (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/info/tempcontrol/";

    try {
        const response = await fetch(url);
        // response.status holds http code

        if (response.ok === true || useTemps === true) {
            sessionStorage.setItem("useTemps", true);
            if (!$('#displaytemplink').is(':visible')) {
                $('#displaytemplink').toggle();
            }
        } else {
            sessionStorage.setItem("useTemps", false);
            if ($('#displaytemplink').is(':visible')) {
                $('#displaytemplink').toggle();
            }
        }
        if (loaded < numReq) {
            loaded++;
        }
    } catch (err) {
        // Can check for (err) here
        setTimeout(chooseTempMenu, 10000);
    }
    if (typeof callback == "function") {
        callback();
    }
}

function checkSemaphore(callback) { // Check to see if the update is complete
    if (!dataHostCheckDone) {
        setTimeout(chooseTempMenu, 10);
        return;
    }

    var url = dataHost;
    if (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/action/ping/";

    var jqxhr = $.get(url)
        .done(function (data) {
            callback(true);
        })
        .fail(function () {
            // This will fail while controller resets
            callback(false);
        });
}

function isIP(hostname) { // Bool: is this an IP address
    if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(hostname)) {
        return (true)
    } else {
        return (false)
    }
}

function isMDNS(hostname) { // Bool: Is this an mDNS host name
    if (hostname.endsWith(".local")) {
        return (true);
    } else {
        return (false);
    }
}

function getUseTemps() {
    // To set temperature link display, use the following syntax in console:
    //
    // >>   localStorage.setItem("useTemps", true);
    //
    useTemps = (JSON.parse(localStorage.getItem("useTemps")) === true);
    if (useTemps) console.info("NOTICE: Using 'useTemps'.");
    // (We have to serialize the vlaue of lical storage to get a bool)
    //
    // Also remember that this must be set to false for things to work
    // normally again:
    //
    // >>   localStorage.setItem("useTemps", false);
    //
}

function checkDataHost() {
    if (localStorage.getItem("dataHost")) {
        // Check to see if we are using a data host or not
        // A "failure" means we are using a data host (true)
        const ping = new XMLHttpRequest();
        ping.open('GET', '/api/v1/action/ping/');

        try {
            ping.send();
            ping.onload = function () {
                if (ping.status != 200) {
                    // To set data server, use the following syntax in console:
                    //
                    // >>   localStorage.setItem("dataHost", "http://kegcop.local/");
                    //
                    // Note that the full URL including 'http:// and
                    // trailing '/' is required
                    //
                    // Will additionally "ping" the api, if it is not found and we have
                    // a datahost, it will use the dataHost.  If the server is found
                    // (i.e. we are connected to the controller and not a dev server) 
                    // then it will ignore devServer.
                    //
                    dataHost = localStorage.getItem("dataHost");
                    if (dataHost && !dataHost.endsWith("/")) dataHost += "/";
                    if (dataHost) console.info("NOTICE: Using 'dataHost' (a single 404 for /api/v1/action/ping/ is normal.)");
                    //
                    // Also remember that this must be cleared for things to work
                    // normally again:
                    //
                    // >>   localStorage.setItem("dataHost", "");
                    //
                } else {
                    dataHost = "";
                    console.info("NOTICE: Not using 'dataHost'.");
                }
                dataHostCheckDone = true;
                if (loaded < numReq) {
                    loaded++;
                }
            };
        } catch (err) {
            console.error("ERROR: 'dataHost' check failed.");
            setTimeout(checkDataHost, 10000);
        }
    } else {
        dataHostCheckDone = true;
        if (loaded < numReq) {
            loaded++;
        }
    }
}

function getEventTarget(event) {
    var targetURL = "";
    // The following is needed because of clickable spans inside an anchor element
    if (!event.target.href) {
        var tempElement = event.target.parentNode;
        while (!tempElement.href && tempElement.parentNode) {
            // Dig for the parent element URL
            tempElement = tempElement.parentNode;
            var type = event.target.type;
        }
        try {
            targetURL = new URL(tempElement.href);
        } catch {
            ;
        }

    } else {
        try {
            targetURL = new URL(event.target.href);
        } catch {
        }
    }
    return targetURL;
}

function cleanURL(tempURL) {
    // TODO:  A 404 keeps the "bad" page as it's href and blows this up.
    targetURL = tempURL; // Yes we're actually going to use both of these
    if (!dataHostCheckDone) {
        setTimeout(chooseTempMenu, 10);
        return;
    }
    // This all exists because we need to re-write URLs when using a dev server
    try {
        const currentURL = new URL(window.location.href);
        try {
            targetURL = new URL(targetURL);
        } catch {
            targetURL = new URL(currentURL.protocol + "//" + currentURL.host + "/" + targetURL);
        }
        var newURL;
        newURL = targetURL.protocol;
        newURL += "//";
        newURL += targetURL.host;
        newURL += "/";

        var newPath = targetURL.pathname;
        while (newPath.startsWith("/")) {
            newPath = newPath.substring(1, newPath.length);
        }
        while (newPath.endsWith("/")) {
            newPath = newPath.substring(0, newPath.length - 1);
        }

        if (dataHost) {
            // If we have a datahost configured:
            if (newPath.includes("/")) newPath = newPath.split('/')[1];
            var oldPath = currentURL.pathname;
            while (oldPath.startsWith("/")) {
                oldPath = oldPath.substring(1, oldPath.length);
            }
            while (oldPath.endsWith("/")) {
                oldPath = oldPath.substring(0, oldPath.length - 1);
            }
            // Retain any actual path name
            if (oldPath.includes("/")) oldPath = oldPath.split('/')[0];
            if (!newPath.endsWith(".htm")) {
                newPath += ".htm";
            }
            newPath = oldPath + "/" + newPath;
        } else {
            newPath += "/";
        }
        newURL += newPath;
        newURL += targetURL.search;
        newURL += targetURL.hash;
        return newURL;
    } catch {
        console.warn("WARNING: Unable to clean URL: " + tempURL);
        return;
    }
}
