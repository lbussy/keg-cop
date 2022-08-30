// Supports Index page

toggleLoader("on");

var numReq = 4 + numReqPre;
var loaded = 0;
var imperial;
var labels = [];
var percent = [];
var capacity = [];
var remaining = [];
var tapChart;
var flowReloadTimer = 5000;
var tempReloadTimer = 10000;
// Calibration mode variables
calValue = 0;
calLineType = '';

function finishLoad() {
    // Catch event from kegcop_pre.js
    chooseTempMenu();
    populateFlow();
    populateConfig();
    populateTemps();
    pollComplete();
}

function populateFlow(callback = null) { // Get flowmeter data
    if (!dataHostCheckDone) {
        setTimeout(populateFlow, 10);
        return;
    }

    var url = dataHost;
    while (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/config/taps/";

    var okToClear = false;
    if (labels.length) { // Clear arrays if we are re-running
        okToClear = true;
    }
    var flow = $.getJSON(url, function () {
        flowAlert.warning();
    })
        .done(function (flow) {
            if (okToClear) {
                labels = [];
                percent = [];
                capacity = [];
                remaining = [];
            }
            var numTaps = flow["taps"].length;
            if (flow.imperial) {
                imperial = true;
            } else {
                imperial = false;
            }
            calLineType = ''; // Clear calibration mode
            try {
                for (var i = 0; i < numTaps; i++) {
                    if (flow.taps[i].active) {
                        labels.push("Tap " + flow.taps[i].label + ": " + flow.taps[i].name);
                        percent.push(((parseFloat(flow.taps[i].remaining) / parseFloat(flow.taps[i].capacity)) * 100));
                        capacity.push(parseFloat(flow.taps[i].capacity));
                        remaining.push(parseFloat(flow.taps[i].remaining));
                    }
                    if (flow.taps[i].calibrating) { // Tap is calibrating
                        calLineType = 'line';
                        calValue = i;
                    }
                }
                if (loaded < numReq) {
                    loaded++;
                }
                if (typeof callback == "function") {
                    callback();
                }
            } catch {
                if (!unloadingState) {
                    flowAlert.warning("Unable to parse flowmeter data.");
                }
                setTimeout(populateFlow, 10000);
            }
        })
        .fail(function () {
            if (!unloadingState) {
                flowAlert.warning("Unable to retrieve flowmeter data.");
            }
            setTimeout(populateFlow, 10000);
        })
        .always(function () {
            // Can post-process here
        });
}

function populateConfig() { // Get configuration settings
    if (!dataHostCheckDone) {
        setTimeout(populateConfig, 10);
        return;
    }

    var url = dataHost;
    while (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/config/settings/";

    var config = $.getJSON(url, function () {
        configAlert.warning();
    })
        .done(function (config) {
            try {
                // Header text
                var headerText = "Taplist for " + config.copconfig.kegeratorname;
                if (config.copconfig.breweryname) {
                    headerText += " in " + config.copconfig.breweryname;
                }
                $('#taplistName').text(headerText);
                if (loaded < numReq) {
                    loaded++;
                }
            } catch {
                if (!unloadingState) {
                    configAlert.warning("Unable to parse configuration data.");
                }
                setTimeout(populateConfig, 10000);
            }
        })
        .fail(function () {
            if (!unloadingState) {
                configAlert.warning("Unable to retrieve configuration data.");
            }
            setTimeout(populateConfig, 10000);

        })
        .always(function () {
            // Can post-process here
        });
}

function populateTemps(callback = null) { // Get current temperature and state
    if (!dataHostCheckDone) {
        setTimeout(populateTemps, 10);
        return;
    }

    var url = dataHost;
    while (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/info/sensors/";

    var config = $.getJSON(url, function () {
        tempAlert.warning();
    })
        .done(function (temps) {
            try {
                if (temps.controlenabled) {
                    // Set control point display
                    $('#controlPoint').text(temps.sensors[temps.controlpoint].name + ":");
                    $('#controlTemp').text(parseFloat(temps.sensors[temps.controlpoint].value).toFixed(1));

                    // Set F or C
                    if (temps.imperial) {
                        $('#tempFormat').html("&#x2109;");
                    } else {
                        $('#tempFormat').html("&#x2103;");
                    }

                    tempStatus(temps); // Populate temperature and fan control display
                }

                if (loaded < numReq) {
                    loaded++;
                }
                if (typeof callback == "function") {
                    callback();
                }
            } catch {
                if (!unloadingState) {
                    tempAlert.warning("Unable to parse temperature data.");
                }
                setTimeout(populateTemps, 10000);
            }
        })
        .fail(function () {
            if (!unloadingState) {
                tempAlert.warning("Unable to retrieve temperature data.");
            }
            setTimeout(populateTemps, 10000);
        })
        .always(function () {
            // Can post-process here
        });
}

function clearState() {
    $("#coolstate").removeClass("alert-warning");
    $("#coolstate").removeClass("alert-danger");
    $("#coolstate").removeClass("alert-success");
    $("#coolstate").removeClass("alert-info");
    $("#coolstate").removeClass("alert-primary");
    $("#coolstate").removeClass("alert-secondary");
    $("#coolstate").removeClass("alert-light");
}

function addData(chart, label, data) {
    //chart.data.labels.push(label);
    chart.data.datasets.forEach((dataset) => {
        dataset.data = data;
    });
    chart.update();
}

function removeData(chart) {
    //chart.data.labels.pop();
    chart.data.datasets.forEach((dataset) => {
        dataset.data.pop();
    });
    chart.update();
}

function doChart() { // Draw chart.js chart
    // Get font color from CSS
    const element = document.querySelector('.chart');
    const style = getComputedStyle(element);
    var fontColor = style.color;

    if (tapChart) {
        tapChart.data.datasets.forEach((dataset) => {
            // Update data
            dataset.data = [];
            dataset.data = percent;
        });
        tapChart.options.annotation.annotations.forEach((annotations) => {
            // Update annotations
            annotations.type = calLineType;
            annotations.value = calValue;
        });
        tapChart.update();
    } else {
        var ctx = document.getElementById("tapChart").getContext("2d");

        tapChart = new Chart(ctx, {
            type: 'horizontalBar',
            data: {
                labels: labels,
                datasets: [{
                    data: percent,
                    borderColor: 'rgba(221, 86, 0, 1)',
                    backgroundColor: 'rgba(221, 86, 0, 0.2)',
                    borderWidth: 2
                }]
            },
            options: {
                legend: false,
                responsive: true,

                title: {
                    display: false
                },

                onClick: barClick,

                tooltips: {
                    displayColors: false,

                    callbacks: {
                        label: toolTip
                    }
                },

                scales: {
                    xAxes: [{
                        ticks: {
                            min: 0,
                            max: 100,
                            fontColor: fontColor,
                            callback: function (value, index, values) {
                                return value + "%";
                            }
                        }
                    }],
                    yAxes: [{
                        ticks: {
                            fontColor: fontColor
                        },
                    }]
                },

                annotation: {
                    annotations: [{
                        type: calLineType,
                        mode: 'horizontal',
                        scaleID: 'y-axis-0',
                        value: calValue,
                        borderColor: 'rgb(199, 28, 34)',
                        borderWidth: 4,
                        label: {
                            enabled: true,
                            content: "In Calibration Mode"
                        }
                    }]
                }
            }
        });
    }
}

function toolTip(tooltipItem, data) { // Callback for tool tips
    var index = tooltipItem.index;
    var label = [];

    // Line 1
    var pct = parseFloat(percent[index]).toFixed(1) + '%:';
    label.push(pct);

    // Line 2
    var large = '';
    large += parseFloat(remaining[index]).toFixed(2);
    large += " of ";
    large += parseFloat(capacity[index]).toFixed(2);
    if (imperial) {
        large += " gallons ";
    } else {
        large += " liters ";
    }
    label.push(large);

    // Line 3
    var small = '';
    small += "(";
    if (imperial) {
        small += (parseFloat(remaining[index]) * 128).toFixed(0);
        small += " fl oz remaining)";
    } else {
        small += (parseFloat(remaining[index]) * 1000).toFixed(0);
        small += " ml remaining)";
    }
    label.push(small);

    return label;
}

function barClick(event, array) { // Bar click handler
    var tapNum = array[0]._index;

    if (!dataHostCheckDone) {
        setTimeout(chooseTempMenu, 10);
        return;
    }

    var url = dataHost;
    while (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/settings/#tap" + tapNum;

    window.open(url, "_self")
}

function flowReload() {
    populateFlow(function callFunction() {
        doChart();
        setTimeout(flowReload, flowReloadTimer);
    });
}

function tempReload() {
    populateTemps(function callFunction() {
        setTimeout(tempReload, tempReloadTimer);
    });
}

function finishPage() { // Display page
    show_ksTV();
    toggleLoader("off");
    doChart();
    setTimeout(tempReload, tempReloadTimer);
    setTimeout(flowReload, flowReloadTimer);
}

function show_ksTV() {
    var thisURL = new URL(window.location.href);
    if (thisURL.pathname.includes("/ks-tv") || thisURL.search.includes("ks-tv")) {
        // Clear header and footer
        document.getElementsByTagName('header')[0].innerHTML = "";
        document.getElementsByTagName('footer')[0].className = "";
        document.getElementsByTagName('footer')[0].innerHTML = "";

        // var app = document.getElementById('indexApp');
        // app.classList = "";
        // app.classList.add("d-flex");
        // app.classList.add("flex-column");
        // app.classList.add("min-vh-100");
        // app.classList.add("justify-content-center");
        // app.classList.add("align-items-center");

        // var card = document.getElementById('sizeMe');
        // card.classList = "";
        // card.classList.add("card");
        // card.classList.add("border-dark");
        // card.classList.add("mb-3");
        // card.classList.add("d-flex");
        // card.classList.add("flex-column");
        // card.classList.add("min-vh-100");
        // card.classList.add("justify-content-center");
        // card.classList.add("align-items-center");
        // card.style.width = '90%';
        // card.style.height = '90%';
    }
}
