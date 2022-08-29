// Supports Temps page

toggleLoader("on");

var imperial;
var loaded = 0;
var numReq = 3 + numReqPre;
var labels = [];
var temperatures = [];
var scaleTemps = [];
var setpoint = 0;
var tempChart;
var chartReloadTimer = 10000; // Reload every 10 seconds

function finishLoad() { // Get page data
    chooseTempMenu();
    populateTemps();
    populateConfig();
    pollComplete();
}

function populateTemps(callback = null) { // Get configuration settings
    if (!dataHostCheckDone) {
        setTimeout(populateTemps, 10);
        return;
    }

    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/info/sensors/";

    var okToClear = false;
    if (labels.length) { // Clear arrays if we are re-running
        okToClear = true;
    }
    var config = $.getJSON(url, function () {
        tempAlert.warning();
    })
        .done(function (temps) {
            try {
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

                tempStatus(temps); // Populate temperature and fan control display

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

function populateConfig() { // Get configuration settings
    if (!dataHostCheckDone) {
        setTimeout(populateConfig, 10);
        return;
    }

    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/config/settings/";

    var config = $.getJSON(url, function () {
        configAlert.warning();
    })
        .done(function (config) {
            try {
                // Header text
                var headerText = 'Temperature Sensors for <a class="no-underline" href="/index/">';
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
    // Get font color from CSS
    const element = document.querySelector('.chart');
    const style = getComputedStyle(element);
    var fontColor = style.color;

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
                    xAxes: [{
                        ticks: {
                            fontColor: fontColor
                        },
                    }],
                    yAxes: [{
                        display: true,
                        ticks: {
                            fontColor: fontColor,
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
    var newURL;
    var newPath = "settings/";
    var newSearch = "";
    var newHash = "#sensorcontrol";
    try {
        const url = new URL(window.location.href);
        if (dataHost) {
            // This all exists because we need to re-write URLs when
            // using a dev server
            newURL = url.protocol
            newURL += "//";
            newURL += url.host;
            if (url.pathname.includes("data")) {
                newPath = "/data/" + newPath;
            }
            newURL += newPath;
            if (newURL.endsWith("/")) {
                newURL = newURL.substring(0, newURL.length - 1);
            }
            if (!newURL.endsWith(".htm")) {
                newURL += ".htm";
            }
            newURL += newSearch;
            newURL += newHash;
        } else {
            newURL = newPath + newHash;
        }
        // Open the rewritten URL and return false to prevent default
        window.open(newURL,"_self")
        return false;
    } catch (error) {
        console.error("Badly formatted URL passed to function.");
        return false;
    }
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
