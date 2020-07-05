// Supports flowmeter calibration sub-page

// Pulses for Tap Calibration
var doFollowPulse = false;
var calSetting = false; // Keep track of whether we are toggling calibration mode
var inCalMode = false;
var pulseReloadTimer = 2000;
var byWeight = false;
var byVolume = false;
var clearCalibrate = false;

// Flowmeter Calibration Events
//
// Toggle by weight/volume controls
$('input[type=radio][name=calbyvolume]').change(function () {
    if (!$("#calbyweight").hasClass("disabled")) {
        if ($('#byunspecified').is(':visible')) {
            $('#byunspecified').toggle(); // If unspecified visible turn it off
        }
        if (this.value == 'true') { // If we are by volume
            if ($('#byweight').is(':visible')) {
                $('#byweight').toggle(); // If weight visible turn it off
            }
            if (!$('#byvolume').is(':visible')) {
                $('#byvolume').toggle(); // If volume not visible, turn it on
            }
        }
        else if (this.value == 'false') {  // If we are by weight
            if ($('#byvolume').is(':visible')) {
                $('#byvolume').toggle(); // If volume visible turn it off
            }
            if (!$('#byweight').is(':visible')) {
                $('#byweight').toggle(); // If not visible, turn it on
            }
        }
    }
});
// Handle click on calibration label
$('#byvolume').on('click', function (event) {
    clickByVolume();
});
$('#byweight').on('click', function (event) {
    clickByWeight();
});
//
$("input[type='reset']").closest('form').on('reset', function (event) { // Reset flow cal form
    resetFlowCalForm();
});

// Handle resetting the calibration form if we leave it
$('a[data-toggle="tab"]').on('shown.bs.tab', function (event) {
    previousTab = currentTab;
    currentTab = $(event.target).text();
    if (previousTab == "Calibrate Flowmeters") {
        doFollowPulse = false; // Turn off calibration mode
        clearCalibrate = true; // Set flag to reset form
    } else if (currentTab == "Calibrate Flowmeters" && clearCalibrate == true) {
        // Clear the calibration form if we were here before
        clearCalibrate = false;
        resetFlowCalForm();
    }
});

function clickByWeight() {
    // Hide previous blocks
    $("#flowmeter").attr("disabled", "disabled");
    $("#calbyweight").attr("disabled", "disabled");
    $("#calbyvolume").attr("disabled", "disabled");
    $("#byweightlabel").addClass("disabled");
    $("#byvolumelabel").addClass("disabled");
    if (!$('#calbyweightblock').is(':visible')) {
        $('#calbyweightblock').toggle(); // Turn on weight calibration
    }
    if (!$('#calsubmit').is(':visible')) {
        $('#calsubmit').toggle(); // Turn on Form
        // Put controller in calibration mode, read pulse count
        doFollowPulse = true;
        followPulses();
    }
    byWeight = true;
    byVolume = false;
}

function clickByVolume() {
    // Hide previous blocks
    $("#flowmeter").attr("disabled", "disabled");
    $("#calbyweight").attr("disabled", "disabled");
    $("#calbyvolume").attr("disabled", "disabled");
    $("#byweightlabel").addClass("disabled");
    $("#byvolumelabel").addClass("disabled");
    if (!$('#calbyvolumeblock').is(':visible')) {
        $('#calbyvolumeblock').toggle(); // Turn on volume calibration
    }
    if (!$('#calsubmit').is(':visible')) {
        $('#calsubmit').toggle(); // Turn on Form
        // Put controller in calibration mode, read pulse count
        doFollowPulse = true;
        followPulses();
    }
    byWeight = false;
    byVolume = true;
}

function followPulses() {
    if (doFollowPulse) {
        if (inCalMode) {
            pulseReload(function callFunction() { // Reload pulses
                setTimeout(followPulses, pulseReloadTimer);
            });
        } else {
            var intervalID = window.setInterval(function () { // Poll every pulseReloadTimer/2 seconds
                if (calSetting == false) {
                    calSetting = true; // Make sure we only run this once
                    toggleCalMode(true, function (semaphore) {
                        if (semaphore == true) {
                            calSetting = false;
                            inCalMode = true;
                            window.clearInterval(intervalID); // Stop checking
                            followPulses();
                        }
                    });
                }
            }, pulseReloadTimer / 2);
        }
    } else {
        toggleCalMode(false, function (semaphore) {
            if (semaphore == true) {
                calSetting = false;
                inCalMode = false;
                window.clearInterval(intervalID); // Stop checking
            }
        });
    }
}

function toggleCalMode(inCal = false, callback = null) {
    var url;
    if (inCal) {
        url = "/setcalmode/";
    } else {
        url = "/clearcalmode/";
    }
    $.ajax({
        url: url,
        type: 'POST',
        success: function (data) {
            if (typeof callback == "function") {
                callback(true);
            }
        },
        error: function (data) {
            if (typeof callback == "function") {
                callback(false);
            }
        },
        complete: function (data) {
            //
        }
    });
}

function resetFlowCalForm() {
    doFollowPulse = false; // Turn off calibration mode
    // Set flowmeter to 1 and enable
    $('#flowmeter').prop('selectedIndex', 0);
    $('#flowmeter').attr("disabled", false);
    // Clear radio and enable
    $('input[id="calbyvolume"]').prop('checked', false);
    $('input[id="calbyvolume"]').attr("disabled", false);
    $('input[id="calbyweight"]').prop('checked', false);
    $('input[id="calbyweight"]').attr("disabled", false);
    // Hide Config by * and show choose mode
    if (!$('#byunspecified').is(':visible')) {
        $('#byunspecified').toggle(); // Turn on disabled button
    }
    if ($('#byweight').is(':visible')) {
        $('#byweight').toggle(); // Turn off weight calibration
    }
    if ($('#byvolume').is(':visible')) {
        $('#byvolume').toggle(); // Turn off volume calibration
    }
    $("#byweightlabel").removeClass("disabled");
    $("#byvolumelabel").removeClass("disabled");
    // Hide form block
    if ($('#calbyweightblock').is(':visible')) {
        $('#calbyweightblock').toggle(); // Turn off weight calibration
    }
    if ($('#calbyvolumeblock').is(':visible')) {
        $('#calbyvolumeblock').toggle(); // Turn off weight calibration
    }
    if ($('#calsubmit').is(':visible')) {
        $('#calsubmit').toggle(); // Turn off Form
    }
    $('#weight').val('');
    // TODO:  Blank out volume
    byWeight = false;
    byVolume = false;
}

function pulseReload(callback = null) { // Get pulses
    var selectedIndex = $('#flowmeter').prop('selectedIndex');
    var url = "/pulses/";
    var pulses = $.getJSON(url, function () {
    })
        .done(function (pulses) {
            var numTaps = pulses.pulses.length;
            try {
                for (var i = 0; i < numTaps; i++) {
                    if (i == selectedIndex) {
                        var pulses = pulses.pulses[i];
                        var weight = $('#weight').val();
                        var volume = $('#volume').val();
                        var unit = 0;
                        var sg = $('#sg').val();

                        $('#pulses').val(parseInt(pulses), 10);

                        if (byWeight) {
                            if (sg == 0) sg = 1;    // Use SG of 1.000 if blank
                            weight = weight / sg;   // Weight adjusted for SG

                            if (imperial && weight > 0) { // Imperial
                                unit = (weight / 1.040842838) / 128; // Convert to fl oz then gallons
                                var ppg = pulses / unit;
                                $('#ppu').val(parseInt(ppg));
                            } else if (!imperial && weight > 0) { // Metric
                                unit = weight / 1000; // Convert to liters
                                var ppl = pulses / unit;
                                $('#ppu').val(parseInt(ppl));
                            }
                        }
                        if (byVolume) {
                            if (imperial) { // Imperial
                                unit = volume / 128; // Convert to gallons
                                var ppg = pulses / unit;
                                $('#ppu').val(parseInt(ppg));
                            } else {        // Metric
                                unit = volume / 1000; // Convert to liters
                                var ppl = pulses / unit;
                                $('#ppu').val(parseInt(ppl));
                            }
                        }
                        if ((pulses > 0) && ($('#ppu').val() > 0)) {
                            // Enable Submit button
                            $('.setppu').prop("disabled", false);
                        }
                    }
                }
                if (typeof callback == "function") {
                    callback();
                }
            }
            catch {
                if (!unloadingState) {
                    alert("Unable to parse flow data from controller.");
                }
            }
        })
        .fail(function () {
            if (!unloadingState) {
                alert("Unable to retrieve flow data from controller.");
            }
        })
        .always(function () {
            // Can post-process here
        }
        );
}

function processTapCalPost(url, obj) {
    toggleLoader("on");
    // Handle tap calibration posts

    // Get form data
    tapnum = $('#flowmeter').val();
    ppu = $('#ppu').val();

    // Process post
    data = {
        tapnum: tapnum,
        ppu: ppu
    }
    postData(url, data, false, true);
    resetFlowCalForm();
}
