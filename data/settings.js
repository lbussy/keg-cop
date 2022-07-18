// Supports Settings page

toggleLoader("on");
var loaded = 0; // Hold data load status
var numReq = 3; // Number of JSON required
var hostname = window.location.hostname;
var originalHostnameConfig;
var imperial;
var hashLoc;
var posted = false;
var numTaps = 0;

// Supports flowmeter calibration sub-page
//
// Pulses for Tap Calibration
var doFollowPulse = false;
var calSetting = false; // Keep track of whether we are toggling calibration mode
var inCalMode = false;
var pulseReloadTimer = 2000;
var byWeight = false;
var byVolume = false;
var clearCalibrate = false;

// Tab tracking
var previousTab = "";
var currentTab = "";

$('a[data-toggle="tab"]').on('shown.bs.tab', function (event) {
    // Actions to take when changing tabs
    previousTab = currentTab;
    currentTab = $(event.target).text();

    // Handle resetting the calibration form if we leave it
    if (previousTab == "Calibrate Flowmeters") {
        doFollowPulse = false; // Turn off calibration mode
        clearCalibrate = true; // Set flag to reset form
    } else if (currentTab == "Calibrate Flowmeters" && clearCalibrate == true) {
        // Clear the calibration form if we were here before
        clearCalibrate = false;
        resetFlowCalForm();
    }
    var url = $(event.target).attr("href") // URL of activated tab
    var hashLoc = url.substring(url.indexOf('#')); // Get hash
    updateHelp(hashLoc); // Set context-sensitive help
});

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

function finishLoad() { // Get page data
    toggleCalMode(false);
    loadHash();
    chooseTempMenu();
    populateConfig();
    populateFlow();
    pollComplete();
}

function repopulatePage(doSpinner = false) { // Reload data if we need it
    if (doSpinner) {
        toggleLoader("on");
    }
    loaded = 0;
    chooseTempMenu();
    populateConfig();
    populateFlow();
    pollComplete();
}

function loadHash() { // Link to tab via hash value
    var url = document.location.toString();
    if (url.match('#')) {
        $('.nav-tabs a[href="#' + url.split('#')[1] + '"]').tab('show');
    }

    // Change hash for page-reload
    $('.nav-tabs a').on('shown.bs.tab', function (e) {
        window.location.hash = e.target.hash;
    });
}

function populateFlow(callback = null) { // Get flowmeter settings
    var url = dataHost + "api/v1/config/taps";
    var flow = $.getJSON(url, function () {
        flowAlert.warning();
    })
        .done(function (flow) {
            numTaps = flow["taps"].length;
            try {
                for (var i = 0; i < numTaps; i++) {
                    // tap1label
                    $('input[name="tap' + i + 'label"]').val(parseInt(flow.taps[i].label), 10);
                    $('input[name="tap' + i + 'taplistioTap"]').val(parseInt(flow.taps[i].taplistioTap), 10);
                    $('input[name="tap' + i + 'ppu"]').val(parseInt(flow.taps[i].ppu), 10);
                    $('input[name="tap' + i + 'bevname"]').val(flow.taps[i].name);
                    $('input[name="tap' + i + 'cap"]').val(parseFloat(flow.taps[i].capacity).toFixed(4));
                    $('input[name="tap' + i + 'remain"]').val(parseFloat(flow.taps[i].remaining).toFixed(4));
                    if (flow.taps[i].active) {
                        $('input[name="tap' + i + 'active"]')[0].checked = true;
                    } else {
                        $('input[name="tap' + i + 'active"]')[1].checked = true;
                    }
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
            if (loaded < numReq) {
                loaded++;
            }
            if (typeof callback == "function") {
                callback();
            }
        });
}

function populateConfig(callback = null) { // Get configuration settings
    var url = dataHost + "api/v1/config/settings";
    var config = $.getJSON(url, function () {
        configAlert.warning()
    })
        .done(function (config) {
            try {
                $('input[name="hostname"]').val(config.copconfig.hostname);
                originalHostnameConfig = $('input[name="hostname"]').val();
                $('input[name="breweryname"]').val(config.copconfig.breweryname);
                $('input[name="kegeratorname"]').val(config.copconfig.kegeratorname);
                $('select[name="controlnum"] option[value=' + parseInt(config.copconfig.controlnum, 10) + ']').attr('selected', 'selected');
                if (config.copconfig.imperial) {
                    imperial = true;
                    $('input:radio[name="imperial"]')[1].checked = true;
                } else {
                    imperial = false;
                    $('input:radio[name="imperial"]')[0].checked = true;
                }
                // if (config.copconfig.serial) {
                //     $('input:radio[name="serial"]')[1].checked = true;
                // } else {
                //     $('input:radio[name="serial"]')[0].checked = true;
                // }
                if (config.copconfig.tapsolenoid) {
                    $('input:radio[name="tapsolenoid"]')[0].checked = true;
                } else {
                    $('input:radio[name="tapsolenoid"]')[1].checked = true;
                }
                $('input[name="setpoint"]').val(parseFloat(config.temps.setpoint).toFixed(1));
                $('select[name="controlpoint"] option[value=' + parseInt(config.temps.controlpoint, 10) + ']').attr('selected', 'selected');
                if (config.temps.controlenabled) {
                    $('input:radio[name="controlenabled"]')[0].checked = true;
                } else {
                    $('input:radio[name="controlenabled"]')[1].checked = true;
                }
                if (config.temps.coolonhigh) {
                    $('input:radio[name="coolonhigh"]')[1].checked = true;
                } else {
                    $('input:radio[name="coolonhigh"]')[0].checked = true;
                }
                if (config.temps.roomenabled) {
                    $('input:radio[name="enableroom"]')[0].checked = true;
                } else {
                    $('input:radio[name="enableroom"]')[1].checked = true;
                }
                $('input[name="roomcal"]').val(parseFloat(config.temps.roomcal).toFixed(2));
                if (config.temps.towerenabled) {
                    $('input:radio[name="enabletower"]')[0].checked = true;
                } else {
                    $('input:radio[name="enabletower"]')[1].checked = true;
                }
                $('input[name="towercal"]').val(parseFloat(config.temps.towercal).toFixed(2));
                if (config.temps.upperenabled) {
                    $('input:radio[name="enableupper"]')[0].checked = true;
                } else {
                    $('input:radio[name="enableupper"]')[1].checked = true;
                }
                $('input[name="uppercal"]').val(parseFloat(config.temps.uppercal).toFixed(2));
                if (config.temps.lowerenabled) {
                    $('input:radio[name="enablelower"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablelower"]')[1].checked = true;
                }
                $('input[name="lowercal"]').val(parseFloat(config.temps.lowercal).toFixed(2));
                if (config.temps.kegenabled) {
                    $('input:radio[name="enablekeg"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablekeg"]')[1].checked = true;
                }
                $('input[name="kegcal"]').val(parseFloat(config.temps.kegcal).toFixed(2));

                $('input[name="kegscreen"]').val(config.kegscreen.url);

                $('input[name="taplistio_venue"]').val(config.taplistio.venue);
                $('input[name="taplistio_secret"]').val(config.taplistio.secret);

                $('input[name="rpintshost"]').val(config.rpintstarget.host);
                $('input[name="rpintsport"]').val(parseInt(config.rpintstarget.port, 10));
                $('input[name="rpintsusername"]').val(config.rpintstarget.username);
                $('input[name="rpintspassword"]').val(config.rpintstarget.password);
                $('input[name="rpintstopic').val(config.rpintstarget.topic);

                $('input[name="targeturl"]').val(config.urltarget.url);
                $('input[name="targetfreq"]').val(parseInt(config.urltarget.freq, 10));
                $('.updatepulsesecs').text(pulseReloadTimer / 1000);
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
            doUnits(); // Set proper units on page
            if (loaded < numReq) {
                loaded++;
            }
            if (typeof callback == "function") {
                callback();
            }
        });
}

function doUnits() { // Change names on page according to units in place
    // Set Gallon / Liter
    if (imperial) {
        $('.setppu').text('Set PPG'); // Flow cal set button
        $('.galliter').text('Gallon'); // Tap config pages
        $('.galliterlower').text('gallon'); // Tap config pages
        $('.ozgrams').text("Ounces");
        $('.ozml').text("Ounces");
        $('.caplong').text('Capacity in Gallons'); // Tap config pages
        $('.remlong').text('Remaining in Gallons'); // Tap config pages
        $('.setfarcel').text('Set point in °F'); // Temp control pages
        $('.ppu').text('PPG'); // Flowmeter calibration pages
    } else {
        $('.setppu').text('Set PPL'); // Flow cal set button
        $('.galliter').text('Liter'); // Tap config pages
        $('.galliterlower').text('liter'); // Tap config pages
        $('.ozgrams').text("Grams");
        $('.ozml').text("Milliliters");
        $('.caplong').text('Capacity in Liters'); // Tap config pages
        $('.remlong').text('Remaining in Liters'); // Tap config pages
        $('.setfarcel').text('Set point in °C'); // Temp control pages
        $('.ppu').text('PPL'); // Flowmeter calibration pages
    }
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

function finishPage() { // Display page
    posted = true;
    toggleTIO();
    toggleLoader("off");
}

// PUT Handlers:

function processPost(obj) {
    posted = false;
    hashLoc = window.location.hash;
    var $form = $(obj);
    url = dataHost + $form.attr("action");

    if (dataHost) {
        alert("DEBUG: Cannot process settings POST when using dataHost.");
        return false;
    }

    $("button[id='submitSettings']").prop('disabled', true);
    $("button[id='submitSettings']").html('<i class="fa fa-spinner fa-spin"></i> Updating');

    // Switch here for hashLoc
    switch (hashLoc) {
        case "#tap0":
            processTapPost(url, obj, 0);
            break;
        case "#tap1":
            processTapPost(url, obj, 1);
            break;
        case "#tap2":
            processTapPost(url, obj, 2);
            break;
        case "#tap3":
            processTapPost(url, obj, 3);
            break;
        case "#tap4":
            processTapPost(url, obj, 4);
            break;
        case "#tap5":
            processTapPost(url, obj, 5);
            break;
        case "#tap6":
            processTapPost(url, obj, 6);
            break;
        case "#tap7":
            processTapPost(url, obj, 7);
            break;
        case "#tap8":
            processTapPost(url, obj, 8);
            break;
        case "#tempcontrol":
            processTempControlPost(url, obj);
            break;
        case "#sensorcontrol":
            processSensorControlPost(url, obj);
            break;
        case "#kegscreen":
            processKegScreenPost(url, obj);
            break;
        case "#taplistio":
            processTaplistIOPost(url, obj);
            break;
        case "#targeturl":
            processTargetUrlPost(url, obj);
            break;
        case "#rpints":
            processRPintsPost(url, obj);
            break;
        case "#controller":
            processControllerPost(url, obj);
            break;
        case "#flowcal":
            processTapCalPost(url, obj);
            break;
        default:
            // Unknown hash location passed
            break;
    }
    buttonClearDelay();
    return false; // Prevent default event
}

function processTapPost(url, obj, tapNum) {
    // Handle tap posts
    var data = {};

    // Get form data
    var $form = $(obj),
        label = $form.find("input[name='tap" + tapNum + "label']").val(),
        ppu = $form.find("input[name='tap" + tapNum + "ppu']").val(),
        bevname = $form.find("input[name='tap" + tapNum + "bevname']").val(),
        cap = $form.find("input[name='tap" + tapNum + "cap']").val(),
        remain = $form.find("input[name='tap" + tapNum + "remain']").val(),
        // tap1taplistioTap
        taplistioTap = $form.find("input[name='tap" + tapNum + "taplistioTap']").val(),
        active = $form.find("input[name='tap" + tapNum + "active']:checked").val();

    // Process put
    data = {
        tap: tapNum,
        label: label,
        ppu: ppu,
        bevname: bevname,
        cap: cap,
        remain: remain,
        taplistioTap: taplistioTap,
        active: active
    }
    putData(url, data);
}

function processControllerPost(url, obj) {
    // Handle controller posts
    var data = {};

    // Get form data
    var $form = $(obj),
        hostnameVal = $form.find("input[name='hostname']").val(),
        brewerynameVal = $form.find("input[name='breweryname']").val(),
        kegeratornameVal = $form.find("input[name='kegeratorname']").val(),
        controlnumVal = $form.find("select[name='controlnum']").val(),
        imperialVal = $("[name='imperial']:checked").val(),
        serialVal = $("[name='serial']:checked").val(),
        tapsolenoidVal = $("[name='tapsolenoid']:checked").val()

    // Hold some data about what we changed
    var reloadpage = false;
    var hostnamechanged = false;
    var unitschanged = false;

    // Decide if we changed units
    if ((imperial) != (imperialVal == 'true')) {
        unitschanged = true;
    } else {
        unitschanged = false;
    }

    // Decide if we accessed via IP or non-mDNS name
    var confirmText = '';
    if (hostnameVal != originalHostnameConfig) {
        hostnamechanged = true;
        if (isIP(hostname)) {
            confirmText = 'You have connected with an IP address. Changing the hostname may have unintended consequences. Do you want to proceed?';
            reloadpage = false;
        } else if (!isMDNS(hostname)) {
            confirmText = 'You are not using an mDNS name. Changing the hostname may have unintended consequences. Do you want to proceed?';
            reloadpage = false;
        } else {
            reloadpage = true;
        }
    }
    if (confirmText && (!confirm(confirmText))) {
        // Bail out on put
        return;
    } else {
        // Process put
        toggleLoader("on");
        originalHostnameConfig = hostnameVal; // Pick up changed host name
        data = {
            hostname: hostnameVal,
            breweryname: brewerynameVal,
            kegeratorname: kegeratornameVal,
            controlnum: controlnumVal,
            imperial: imperialVal,
            serial: serialVal,
            tapsolenoid: tapsolenoidVal,
        }
        if (hostnamechanged && reloadpage) {
            var protocol = window.location.protocol;
            var path = window.location.pathname;
            var newpage = protocol + "" + hostnameVal + ".local" + path + hashLoc;
            putData(url, data, newpage);
        } else if (unitschanged) {
            putData(url, data, false, true);
        } else {
            putData(url, data, false, false, function () {
                toggleLoader("off");
            });
        }
    }
}

function processTempControlPost(url, obj) {
    // Handle temperature control posts
    var data = {};

    // Get form data
    var $form = $(obj),
        setpoint = $form.find("input[name='setpoint']").val(),
        controlpoint = $form.find("select[name='controlpoint']").val(),
        controlenabled = $form.find("input[name='controlenabled']:checked").val();
    coolonhigh = $form.find("input[name='coolonhigh']:checked").val();

    // Process put
    data = {
        setpoint: setpoint,
        controlpoint: controlpoint,
        controlenabled: controlenabled,
        coolonhigh: coolonhigh
    }
    putData(url, data);
}

function processSensorControlPost(url, obj) {
    // Handle sensor control posts
    var data = {};

    // Get form data
    var $form = $(obj),
        roomcal = $form.find("input[name='roomcal']").val(),
        enableroom = $form.find("input[name='enableroom']:checked").val(),
        towercal = $form.find("input[name='towercal']").val(),
        enabletower = $form.find("input[name='enabletower']:checked").val(),
        uppercal = $form.find("input[name='uppercal']").val(),
        enableupper = $form.find("input[name='enableupper']:checked").val(),
        lowercal = $form.find("input[name='lowercal']").val(),
        enablelower = $form.find("input[name='enablelower']:checked").val(),
        kegcal = $form.find("input[name='kegcal']").val(),
        enablekeg = $form.find("input[name='enablekeg']:checked").val();

    // Process put
    data = {
        roomcal: roomcal,
        enableroom: enableroom,
        towercal: towercal,
        enabletower: enabletower,
        uppercal: uppercal,
        enableupper: enableupper,
        lowercal: lowercal,
        enablelower: enablelower,
        kegcal: kegcal,
        enablekeg: enablekeg
    }
    putData(url, data, true, true);
}

function processKegScreenPost(url, obj) {
    // Handle KegScreen Name
    var data = {};

    // Get form data
    var $form = $(obj),
        kegscreen = $form.find("input[name='kegscreen']").val(),

        // Process put
        data = {
            kegscreen: kegscreen
        };
    putData(url, data);
}

function processTaplistIOPost(url, obj) {
    // Handle Keg Screen Name
    var data = {};

    // Get form data
    var $form = $(obj),
        taplistio_venue = $form.find("input[name='taplistio_venue']").val(),
        taplistio_secret = $form.find("input[name='taplistio_secret']").val(),

        // Process put
        data = {
            taplistio_venue: taplistio_venue,
            taplistio_secret: taplistio_secret
        };
    putData(url, data);
}

function processTargetUrlPost(url, obj) {
    // Handle target URL posts
    var data = {};

    // Get form data
    var $form = $(obj),
        targeturl = $form.find("input[name='targeturl']").val(),
        targetfreq = $form.find("input[name='targetfreq']").val();

    // Process put
    data = {
        targeturl: targeturl,
        targetfreq: targetfreq
    };
    putData(url, data);
}

function processRPintsPost(url, obj) {
    // Handle target URL posts
    var data = {};

    // Get form data
    var $form = $(obj),
        rpintshost = $form.find("input[name='rpintshost']").val(),
        rpintsport = $form.find("input[name='rpintsport']").val();
    rpintsusername = $form.find("input[name='rpintsusername']").val();
    rpintspassword = $form.find("input[name='rpintspassword']").val();
    rpintstopic = $form.find("input[name='rpintstopic']").val();

    // Process put
    data = {
        rpintshost: rpintshost,
        rpintsport: rpintsport,
        rpintsusername: rpintsusername,
        rpintspassword: rpintspassword,
        rpintstopic: rpintstopic
    };
    putData(url, data);
}

function putData(url, data, newpage = false, newdata = false, callback = null) {
    var loadNew = (newpage.length > 0);
    $.ajax({
        url: url,
        type: 'PUT',
        data: data,
        success: function (data) {
            settingsAlert.error();
        },
        error: function (data) {
            settingsAlert.error("Settings update failed.");
        },
        complete: function (data) {
            if (loadNew) {
                window.location.href = newpage;
            } else if (newdata) {
                repopulatePage(true);
            }
            posted = true;
            if (typeof callback == "function") {
                callback();
            }
        }
    });
}

function buttonClearDelay() { // Poll to see if entire page is loaded
    if (posted) {
        $("button[id='submitSettings']").prop('disabled', false);
        $("button[id='submitSettings']").html('Update');
        toggleTIO();
        posted = false;
    } else {
        setTimeout(buttonClearDelay, 500); // try again in 300 milliseconds
    }
}

function updateHelp(hashLoc) {
    var url = "https://docs.kegcop.com/"

    // Switch here for hashLoc
    switch (hashLoc) {
        case "#tap0":
        case "#tap1":
        case "#tap2":
        case "#tap3":
        case "#tap4":
        case "#tap5":
        case "#tap6":
        case "#tap7":
        case "#tap8":
            url = url + "en/latest/context/settings/taps/index.html";
            break;
        case "#tempcontrol":
            url = url + "en/latest/context/settings/temperature/control/index.html";
            break;
        case "#sensorcontrol":
            url = url + "en/latest/context/settings/temperature/sensors/index.html";
            break;
        case "#kegscreen":
            url = url + "en/latest/context/settings/targets/kegscreen/index.html";
            break;
        case "#targeturl":
            url = url + "en/latest/context/settings/targets/url/index.html";
            break;
        case "#rpints":
            url = url + "en/latest/context/settings/targets/rpints/index.html";
            break;
        case "#controller":
            url = url + "en/latest/context/settings/controller/index.html";
            break;
        case "#flowcal":
            url = url + "en/latest/context/settings/advanced/calibrate/index.html";
            break;
        default:
            // Unknown hash location passed
            break;
    }
    $("#contexthelp").prop("href", url)
}

function toggleTIO() {
    var display = "none";
    var tempVenue = $('input[name="taplistio_venue"]').val();
    var tempSecret = $('input[name="taplistio_venue"]').val();

    if (tempVenue && tempSecret) {
        display = "block";
    } else {
        display = "none";
    }

    for (var i = 0; i < numTaps; i++) { // Show/hide Taplist.io tap number
        document.getElementById('taplist.io_' + i).style.display = display;
    }
}

// Calibration Functions

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
        if (inCalMode) { // If we are already in calibration mode
            pulseReload(function callFunction() { // Reload pulses
                setTimeout(followPulses, pulseReloadTimer);
            });
        } else { // We are not yet in calibration mode
            var intervalID = window.setInterval(function () { // Poll every pulseReloadTimer/2 seconds
                if (calSetting == false) { // Run only if we have not run it once
                    calSetting = true; // Make sure we only run this once
                    var selectedIndex = $('#flowmeter').prop('selectedIndex');
                    toggleCalMode(true, selectedIndex, function (semaphore) {
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
        toggleCalMode(false, 0, function (semaphore) {
            if (semaphore == true) {
                calSetting = false;
                inCalMode = false;
                window.clearInterval(intervalID); // Stop checking
            }
        });
    }
}

function toggleCalMode(inCal = false, meter, callback = null) {
    var url = dataHost;
    if (dataHost) {
        alert("DEBUG: Cannot process set/clearcalmode POST when using dataHost.");
        return;
    }
    var data = {};
    if (inCal) {
        url += "api/v1/action/setcalmode/";
        // Get form data
        tapnum = $('#flowmeter').val();
        data = {
            tapnum: tapnum
        }
    } else {
        url += "api/v1/action/clearcalmode/";
    }

    putData(url, data, false, false, function () {
        if (typeof callback == "function") {
            callback(true);
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
    $('#ppu').val('');
    $('#pulses').val('');
    byWeight = false;
    byVolume = false;
}

function pulseReload(callback = null) { // Get pulses
    var selectedIndex = $('#flowmeter').prop('selectedIndex');
    var url = dataHost + "api/v1/info/pulses";
    var pulses = $.getJSON(url, function () {
        flowAlert.warning();
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
        }
        );
}

function processTapCalPost(url, obj) {
    toggleLoader("on");
    // Handle tap calibration posts

    // Get form data
    tapnum = $('#flowmeter').val();
    ppu = $('#ppu').val();

    // Process put
    data = {
        tapnum: tapnum,
        ppu: ppu
    }
    putData(url, data, false, true);
    resetFlowCalForm();
}
