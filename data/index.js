// Supports Index page

toggleLoader("on");
var numReq = 4;
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
    populateTemp();
    pollComplete();
}

function populateFlow(callback = null) { // Get flowmeter data
    var url = dataHost + "api/v1/config/taps";
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
            }
            catch {
                if (!unloadingState) {
                    flowAlert.warning("Unable to parse flowmeter data.");
                }
            }
        })
        .fail(function () {
            if (!unloadingState) {
                flowAlert.warning("Unable to retrieve flowmeter data.");
            }
        })
        .always(function () {
            // Can post-process here
        });
}

function populateConfig() { // Get configuration settings
    var url = dataHost + "api/v1/config/settings";
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

                loaded++;
            }
            catch {
                if (!unloadingState) {
                    configAlert.warning("Unable to parse configuration data.");
                }
            }
        })
        .fail(function () {
            if (!unloadingState) {
                configAlert.warning("Unable to retrieve configuration data.");
            }
        })
        .always(function () {
            // Can post-process here
        });
}

function populateTemp(callback = null) { // Get current temperature and state
    var url = dataHost + "api/v1/info/sensors";
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

                    // Set indicator button
                    switch (temps.status) {
                        case 0: // TSTAT_INACTIVE
                            clearState();
                            $("#coolstate").addClass("alert-secondary");
                            $("#coolstatetooltip").attr("data-original-title", "Thermostat is disabled");
                            break;
                        case 1: // TSTAT_COOL_BEGIN
                            clearState();
                            $("#coolstate").addClass("alert-info");
                            $("#coolstatetooltip").attr("data-original-title", "Thermostat is starting to cool");
                            break;
                        case 2: // TSTAT_COOL_MINOFF
                            clearState();
                            $("#coolstate").addClass("alert-danger");
                            $("#coolstatetooltip").attr("data-original-title", "Thermostat is calling for cooling but in minimum off time");
                            break;
                        case 3: // TSTAT_COOL_ACTIVE
                            clearState();
                            $("#coolstate").addClass("alert-primary");
                            $("#coolstatetooltip").attr("data-original-title", "Thermostat is actively cooling");
                            break;
                        case 4: // TSTAT_IDLE_END
                            clearState();
                            $("#coolstate").addClass("alert-warning");
                            $("#coolstatetooltip").attr("data-original-title", "Thermostat is not calling for cooling, minimum off time ending");
                            break;
                        case 5: // TSTAT_IDLE_MINON
                            clearState();
                            $("#coolstate").addClass("alert-success");
                            $("#coolstatetooltip").attr("data-original-title", "Thermostat is not calling for cooling but in minimum on time");
                            break;
                        case 6: // TSTAT_IDLE_INACTIVE
                            clearState();
                            $("#coolstate").addClass("alert-light");
                            $("#coolstatetooltip").attr("data-original-title", "Thermostat is not calling for cooling, in idle mode");
                            break;
                        case 7: // TSTAT_UNKNOWN
                            clearState();
                            $("#coolstate").addClass("alert-light");
                            $("#coolstatetooltip").attr("data-original-title", "Thermostat is in an unknown state");
                            break;
                        default: // TSTAT_UNKNOWN
                            clearState();
                            $("#coolstate").addClass("alert-light");
                            $("#coolstatetooltip").attr("data-original-title", "Thermostat is in an unknown state");
                            break;
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
                    tempAlert.warning("Unable to parse temperature data.");
                }
            }
        })
        .fail(function () {
            if (!unloadingState) {
                tempAlert.warning("Unable to retrieve temperature data.");
            }
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
                            callback: function (value, index, values) {
                                return value + "%";
                            }
                        }
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
    var url = "settings#tap" + tapNum;
    alert(url);
    window.open(url, "_self")
}

function flowReload() {
    populateFlow(function callFunction() {
        doChart();
        setTimeout(flowReload, flowReloadTimer);
    });
}

function tempReload() {
    populateTemp(function callFunction() {
        setTimeout(tempReload, tempReloadTimer);
    });
}

function finishPage() { // Display page
    toggleLoader("off");
    doChart();
    setTimeout(tempReload, tempReloadTimer);
    setTimeout(flowReload, flowReloadTimer);
}
