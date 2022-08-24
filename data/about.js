// Supports About Page

toggleLoader("off");
var numReq = 5;
var loaded = 0;
var aboutReloadTimer = 60000;

function finishLoad() {
    // Catch page finished event from kegcop_pre.js
    chooseTempMenu();
    populatePage();
}

function populatePage() { // Get page data
    heapToolTip();      // Set up tooltip for debug info
    loadThisVersion();  // Populate form with controller settings
    loadUptime();       // Load uptime information
    loadHeap();         // Load heap information
    loadResetReason();  // Load last reset reason
    pollComplete();
}

function heapToolTip() {
    var heapToolTip = "Heap Information:<br>";
    heapToolTip += "<ul>";
    heapToolTip += "<li>Free Heap = Total free bytes in the heap";
    heapToolTip += "<li>Max = Size of largest free block in the heap";
    heapToolTip += "<li>Frags = 100 - (max * 100) / free";
    heapToolTip += "</ul>";
    $("#uptime").attr("data-original-title", "Time since last controller (re)start");
    $("#resetreason").attr("data-original-title", "Reason for last (re)start");
    $("#heap").attr("data-original-title", heapToolTip);
}

function loadThisVersion() { // Get current parameters
    if (!dataHostCheckDone) {
        setTimeout(loadThisVersion, 10);
        return;
    }

    var url = dataHost;
    if (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/info/thisVersion/";

    var thisVersion = $.getJSON(url, function () {
    })
        .done(function (thisVersion) {
            try {
                $('#thisVersion').text("v" + thisVersion.fw_version);
                $('#thisBranch').text(thisVersion.branch);
                $('#thisBuild').text(thisVersion.build);
                if (loaded < numReq) {
                    loaded++;
                }
            }
            catch {
                $('#thisVersion').html("").html('<span class="text-danger">Error parsing version.</span>');
                $('#thisBranch').text();
                $('#thisBuild').text();
                setTimeout(loadThisVersion, 10000);
            }
        })
        .fail(function () {
            $('#thisVersion').html("").html('<span class="text-danger">Error loading version.</span>');
            $('#thisBranch').text();
            $('#thisBuild').text();
            setTimeout(loadThisVersion, 10000);
        })
        .always(function () {
            // Can post-process here
        });
}

function loadUptime(callback = null) { // Get uptime information
    if (!dataHostCheckDone) {
        setTimeout(loadUptime, 10);
        return;
    }

    var url = dataHost;
    if (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/info/uptime/";

    var uptime = $.getJSON(url, function () {
    })
        .done(function (uptime) {
            try {
                var days = uptime.u.days.toString();
                var hours = uptime.u.hours.toString();
                var minutes = uptime.u.minutes.toString();
                var seconds = uptime.u.seconds.toString();

                var uptime = "Days: " + days + ", Hours: " + hours + ", Minutes: " + minutes + ", Seconds: " + seconds;
                $('#uptime').text(uptime);
                if (loaded < numReq) {
                    loaded++;
                }
            }
            catch {
                $('#uptime').text("(Error parsing uptime.)");
                setTimeout(loadUptime, 10000);
            }
        })
        .fail(function () {
            $('#uptime').text("(Error loading uptime.)");
            setTimeout(loadUptime, 10000);
        })
        .always(function () {
            if (typeof callback == "function") {
                callback();
            }
        });
}

function loadHeap(callback = null) { // Get heap information
    if (!dataHostCheckDone) {
        setTimeout(loadHeap, 10);
        return;
    }

    var url = dataHost;
    if (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/info/heap/";

    var heap = $.getJSON(url, function () {
    })
        .done(function (heap) {
            try {
                var free = heap.h.free;
                var max = heap.h.max;
                var frag = heap.h.frag;

                var heapinfo = "Free Heap: " + free + ", Max: " + max + ", Frags: " + frag;
                $('#heap').text(heapinfo);
                if (loaded < numReq) {
                    loaded++;
                }
            }
            catch {
                $('#heap').text("(Error parsing heap.)");
                setTimeout(loadHeap, 10000);
            }
        })
        .fail(function () {
            $('#heap').text("(Error loading heap.)");
            setTimeout(loadHeap, 10000);
        })
        .always(function () {
            if (typeof callback == "function") {
                callback();
            }
        });
}

function loadResetReason(callback = null) { // Get last reset reason
    if (!dataHostCheckDone) {
        setTimeout(loadResetReason, 10);
        return;
    }

    var url = dataHost;
    if (url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/info/resetreason/";

    var reset = $.getJSON(url, function () {
    })
        .done(function (reset) {
            try {
                var resetReason = reset.r.reason;
                var resetDescription = reset.r.description;

                var resetText = "Reason: " + resetReason + ", Description: " + resetDescription;
                $('#resetreason').text(resetText);
                if (loaded < numReq) {
                    loaded++;
                }
            }
            catch {
                $('#resetreason').text("(Error parsing version.)");
                setTimeout(loadResetReason, 10000);
            }
        })
        .fail(function () {
            $('#resetreason').text("(Error loading version.)");
            setTimeout(loadResetReason, 10000);
        })
        .always(function () {
            if (typeof callback == "function") {
                callback();
            }
        });
}

function uptimeReload() {
    loadUptime(function callFunction() {
        setTimeout(uptimeReload, aboutReloadTimer);
    });
}

function heapReload() {
    loadHeap(function callFunction() {
        setTimeout(heapReload, aboutReloadTimer);
    });
}

function reasonReload() {
    loadResetReason(function callFunction() {
        setTimeout(reasonReload, aboutReloadTimer);
    });
}

function finishPage() { // Display page
    setTimeout(heapReload, aboutReloadTimer);
    setTimeout(uptimeReload, aboutReloadTimer);
    setTimeout(reasonReload, aboutReloadTimer);
}
