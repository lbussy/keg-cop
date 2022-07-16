// Supports Index page

toggleLoader("on");
var numReq = 3;
var loaded = 0;
var imperial;
var labels = [];
var percent = [];
var capacity = [];
var remaining = [];
var tapChart;
var flowReloadTimer = 5000;
var tempsReloadTimer = 10000;
// Calibration mode variables
calValue = 0;
calLineType = '';

function finishLoad() {
    // Catch event from kegcop.js
    populateFlow();
    populateTemps();
    populateConfig();
    pollComplete();
}

function populateFlow(callback = null) { // Get flowmeter data
    var url = thisHost + "api/v1/config/taps";
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
    var url = thisHost + "api/v1/config/settings";
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
    var url = thisHost + "settings/#tap" + tapNum;
    window.open(url, "_self")
}

function flowReload() {
    populateFlow(function callFunction() {
        doChart();
        setTimeout(flowReload, flowReloadTimer);
    });
}

function tempsReload() {
    populateTemps(function callFunction() {
        setTimeout(tempsReload, tempsReloadTimer);
    });
}

function finishPage() { // Display page
    toggleLoader("off");
    doChart();
    setTimeout(tempsReload, tempsReloadTimer);
    setTimeout(flowReload, flowReloadTimer);
}
