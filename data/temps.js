// Supports temps page

toggleLoader("on");
var unloadingState = false;
var imperial;
var loaded = 0;
var numReq = 2;
var labels = [];
var temperatures = [];
var scaleTemps = [];
var setpoint = 0;
var tempChart;
var chartReloadTimer = 10000; // Reload every 10 seconds

// Detect unloading state during getJSON
$(window).bind("beforeunload", function () {
    unloadingState = true;
});

function populatePage() { // Get page data
    $(document).tooltip({ // Enable tooltips
        'selector': '[data-toggle=tooltip]',
        'placement': 'left',
        'toggleEnabled': true
    });
    populateTemps();
    populateConfig();
    pollComplete();
}

function populateConfig() { // Get configuration settings
    var url = "/api/configuration/settings/";
    var config = $.getJSON(url, function () {
        configAlert.warning();
    })
        .done(function (config) {
            try {
                // Header text
                var headerText = 'Temperature Sensors for <a class="no-underline" href="/">';
                headerText += config.copconfig.kegeratorname + '</a>';
                $('#templistName').html(headerText);

                if (loaded < numReq) {
                    loaded++;
                }
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

function populateTemps(callback = null) { // Get configuration settings
    var url = "/api/info/sensors/";
    var okToClear = false;
    if (labels.length) { // Clear arrays if we are re-running
        okToClear = true;
    }
    var config = $.getJSON(url, function () {
        tempAlert.warning();
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

                if (okToClear) {
                    labels = [];
                    temperatures = [];
                    scaleTemps = [];
                }

                if (temps.imperial) {
                    imperial = true;
                } else {
                    imperial = false;
                }

                var length = Object.keys(temps.sensors).length
                var i;
                for (i = 0; i < length; i++) {
                    if (temps.sensors[i].enable) {
                        labels.push(temps.sensors[i].name);
                        temperatures.push(parseFloat(temps.sensors[i].value));
                    }
                }

                $('#controlPoint').text(temps.sensors[temps.controlpoint].name);

                setpoint = parseFloat(temps.setting).toFixed(1);
                setpointLabel = "Setpoint: " + setpoint;
                if (imperial) {
                    setpointLabel += " ℉";
                } else {
                    setpointLabel += " ℃";
                }

                scaleTemps = temperatures;
                scaleTemps.push(parseFloat(setpoint));

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
                tempAlert.warning("Unable to retrieve temperatire data.");
            }
        })
        .always(function () {
            // Can post-process here
        });
}

function updateScales(chart) {
    var yScale = chart.scales['y-axis-0'];
    chart.options.scales = {
        yAxes: [{
            display: true,
            ticks: {
                min: Math.floor(Math.min.apply(this, scaleTemps) - 2),
                max: Math.ceil(Math.max.apply(this, scaleTemps) + 1),
                callback: function (value, index, values) {
                    return value + "°";
                }
            }
        }]
    };
    chart.update();
    yScale = chart.scales['y-axis-0'];
}

function doChart() { // Draw chart.js chart
    if (tempChart) {
        tempChart.data.datasets.forEach((dataset) => {
            // Update data
            dataset.data = [];
            dataset.data = temperatures;
        });
        tempChart.options.annotation.annotations.forEach((annotations) => {
            // Update annotations
            annotations.label.content = setpointLabel;
            annotations.value = setpoint;
        });
        updateScales(tempChart);
        tempChart.update();
    } else {
        var ctx = document.getElementById("tempChart").getContext("2d");

        tempChart = new Chart(ctx, {
            type: 'bar',
            data: {
                labels: labels,
                datasets: [{
                    data: temperatures,
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
                    // yAlign: 'top',
                    displayColors: false,

                    callbacks: {
                        label: toolTip
                    }
                },

                scales: {
                    yAxes: [{
                        display: true,
                        ticks: {
                            min: Math.floor(Math.min.apply(this, scaleTemps) - 2),
                            max: Math.ceil(Math.max.apply(this, scaleTemps) + 1),
                            callback: function (value, index, values) {
                                return value + "°";
                            }
                        }
                    }]
                },

                annotation: {
                    annotations: [{
                        type: 'line',
                        mode: 'horizontal',
                        scaleID: 'y-axis-0',
                        value: setpoint,
                        borderColor: 'rgb(75, 192, 192)',
                        borderWidth: 4,
                        label: {
                            enabled: true,
                            content: setpointLabel
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

    var tempLine = '';
    if (temperatures[index]) {
        tempLine = (temperatures[index]).toFixed(1);
        if (imperial) {
            tempLine += " ℉";
        } else {
            tempLine += " ℃";
        }
        label.push(tempLine);
    }

    return label;
}

function barClick(event, array) { // Bar click handler
    var tapNum = array[0]._index;
    var url = "/settings/#sensorcontrol";
    window.open(url, "_self");
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

function pollComplete() {
    if (loaded == numReq) {
        finishPage();
    } else {
        setTimeout(pollComplete, 300); // try again in 300 milliseconds
    }
}

function chartReload() {
    populateTemps(function callFunction() {
        doChart();
        setTimeout(chartReload, chartReloadTimer);
    });
}

function finishPage() { // Display page
    toggleLoader("off");
    doChart();
    setTimeout(chartReload, chartReloadTimer);
}
