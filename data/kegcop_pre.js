// Common file/functions for all Keg Cop pages

var dataHost = "";
var useTemps = false;

getDataHost();

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
    var newURL;
    try {
        const url = new URL(event.target.href);
        if (dataHost || isIP(url.hostname)) {
            // This all exists because we need to re-write URLs when
            // using a dev server
            newURL = url.protocol
            newURL += "//";
            newURL += url.host;
            newURL += url.pathname;
            if (url.pathname.endsWith("/")) {
                newURL = url.pathname.substring(0, url.pathname.length - 1);
            }
            if (!url.pathname.endsWith(".htm")) {
                newURL += ".htm";
            }
            newURL += url.search;
            newURL += url.hash;
            // Open the rewritten URL and return false to prevent default
            window.open(newURL,"_self");
            return false;
        } else {
            const url = new URL(event.target.href);
            newURL = url.pathname;
            if (newURL.endsWith("/")) {
                newURL = newURL.substring(0, newURL.length - 1);
            }
            if (newURL.startsWith("/")) {
                newURL = newURL.substring(1, newURL.length);
            }
            if (newURL.includes("/")) newURL = "/" + newURL.split('/')[1];
            newURL += "/";
            newURL += url.search;
            newURL += url.hash;
            window.open(newURL,"_self");
            return false;
        }
        // This should be the default handler, return (true) to take default
        return;
    } catch (error) {
        // Bad URL or click in a non-href area
        return;
    }
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
        if (loaded < numReq) setTimeout(chooseTempMenu, 10000);
    }
    if (typeof callback == "function") {
        callback();
    }
}

function checkSemaphore(callback) { // Check to see if the update is complete
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

function getDataHost() {
    // Check for devServer setup

    // To set data server, use the following syntax in console:
    //
    // >>   localStorage.setItem("dataHost", "http://kegcop.local/");
    //
    // Note that the full URL including 'http:// and
    // trailing '/' is required
    //
    dataHost = localStorage.getItem("dataHost") || "";
    if (dataHost) console.info("NOTICE: Using 'dataHost'.");
    //
    // Also remember that this must be cleared for things to work
    // normally again:
    //
    // >>   localStorage.setItem("dataHost", "");
    //

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
