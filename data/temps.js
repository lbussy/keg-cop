// Supports Temps page

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

function finishLoad() { // Get page data
    populateTemps();
    populateConfig();
    pollComplete();
}

function populateConfig() { // Get configuration settings
    var url = dataHost + "api/v1/config/settings";
    var config = $.getJSON(url, function () {
        configAlert.warning();
    })
        .done(function (config) {
            try {
                // Header text
                var headerText = 'Temperature Sensors for <a class="no-underline" href="index">';
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
    var url = dataHost + "settings/#sensorcontrol";
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
