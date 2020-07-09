// Supports temps page

toggleLoader("on");
var imperial;
var loaded = 0;
var numReq = 2;
var labels = [];
var temperatures = [];
var scaleTemps = [];
var setpoint = 0;
var tempChart;
var chartReloadTimer = 10000; // Reload every 10 seconds

function populatePage() { // Get page data
    populateTemps();
    populateConfig();
    pollComplete();
}

function populateConfig() { // Get configuration settings
    var url = "/config/";
    var config = $.getJSON(url, function () {
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
                alert("Unable to parse configuration data from SPIFFS.");
            }
        })
        .fail(function () {
            alert("Unable to retrieve configuration data from SPIFFS.");
        })
        .always(function () {
            // Can post-process here
        });
}

function populateTemps(callback = null) { // Get configuration settings
    var url = "/sensors/";
    var okToClear = false;
    if (labels.length) { // Clear arrays if we are re-running
        okToClear = true;
    }
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

                if (loaded < numReq) {
                    loaded++;
                }
                if (typeof callback == "function") {
                    callback();
                }
            }
            catch {
                alert("Unable to parse temperature data from SPIFFS.");
            }
        })
        .fail(function () {
            alert("Unable to retrieve temperature data from SPIFFS.");
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
