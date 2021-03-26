// Supports settings page

toggleLoader("on");
var unloadingState = false;
var loaded = 0; // Hold data load status
var numReq = 3; // Number of JSON required
var hostname = window.location.hostname;
var originalHostnameConfig;
var imperial;
var hashLoc;
var posted = false;

// Tab tracking
var previousTab = "";
var currentTab = "";

// Handle unloading page while making a getJSON call
$(window).bind("beforeunload", function () {
    unloadingState = true;
});

$('a[data-toggle="tab"]').on('shown.bs.tab', function (event) {
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

// Turn off tooltips on radio button change {" "}
$('input[type=radio]').change(function () {
    $('[data-toggle="tooltip"], .tooltip').tooltip("hide");
});

function populatePage() { // Get page data
    $(document).tooltip({
        'selector': '[data-toggle=tooltip]',
        'toggleEnabled': true
    });
    toggleCalMode(false);
    populateConfig();
    populateFlow();
    populateTemps();
    loadHash();
    pollComplete();
}

function repopulatePage(doSpinner = false) { // Reload data if we need it
    if (doSpinner) {
        toggleLoader("on");
    }
    loaded = 0;
    populateConfig();
    populateFlow();
    populateTemps();
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
    var url = "/flow/";
    var flow = $.getJSON(url, function () {
        flowAlert.warning();
    })
        .done(function (flow) {
            var numTaps = flow["taps"].length;
            try {
                for (var i = 0; i < numTaps; i++) {
                    $('input[name="tap' + i + 'ppu"]').val(parseInt(flow.taps[i].ppu), 10);
                    $('input[name="tap' + i + 'beername"]').val(flow.taps[i].name);
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
    var url = "/config/";
    var config = $.getJSON(url, function () {
        configAlert.warning()
    })
        .done(function (config) {
            try {
                $('input[name="hostname"]').val(config.hostname);
                originalHostnameConfig = $('input[name="hostname"]').val();
                $('input[name="breweryname"]').val(config.copconfig.breweryname);
                $('input[name="kegeratorname"]').val(config.copconfig.kegeratorname);
                if (config.copconfig.imperial) {
                    imperial = true;
                    $('input:radio[name="imperial"]')[1].checked = true;
                } else {
                    imperial = false;
                    $('input:radio[name="imperial"]')[0].checked = true;
                }
                if (config.copconfig.serial) {
                    $('input:radio[name="serial"]')[1].checked = true;
                } else {
                    $('input:radio[name="serial"]')[0].checked = true;
                }
                if (config.copconfig.tapsolenoid) {
                    $('input:radio[name="tapsolenoid"]')[0].checked = true;
                } else {
                    $('input:radio[name="tapsolenoid"]')[1].checked = true;
                }
                $('input[name="setpoint"]').val(parseFloat(config.temps.setpoint).toFixed(1));
                $('select[name="controlpoint"] option[value=' + parseInt(config.temps.controlpoint, 10) + ']').attr('selected', 'selected');
                if (config.temps.controlenabled) {
                    $('input:radio[name="enablecontrol"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablecontrol"]')[1].checked = true;
                }

                if (config.temps.roomenabled) {
                    $('input:radio[name="enableroom"]')[0].checked = true;
                } else {
                    $('input:radio[name="enableroom"]')[1].checked = true;
                }
                $('input[name="calroom"]').val(parseFloat(config.temps.room).toFixed(2));
                if (config.temps.towerenabled) {
                    $('input:radio[name="enabletower"]')[0].checked = true;
                } else {
                    $('input:radio[name="enabletower"]')[1].checked = true;
                }
                $('input[name="caltower"]').val(parseFloat(config.temps.tower).toFixed(2));
                if (config.temps.upperenabled) {
                    $('input:radio[name="enableupper"]')[0].checked = true;
                } else {
                    $('input:radio[name="enableupper"]')[1].checked = true;
                }
                $('input[name="calupper"]').val(parseFloat(config.temps.upper).toFixed(2));
                if (config.temps.lowerenabled) {
                    $('input:radio[name="enablelower"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablelower"]')[1].checked = true;
                }
                $('input[name="callower"]').val(parseFloat(config.temps.lower).toFixed(2));
                if (config.temps.kegenabled) {
                    $('input:radio[name="enablekeg"]')[0].checked = true;
                } else {
                    $('input:radio[name="enablekeg"]')[1].checked = true;
                }
                $('input[name="calkeg"]').val(parseFloat(config.temps.keg).toFixed(2));

                $('input[name="kegscreen"]').val(config.kegscreen.url);

                $('input[name="mqtthost"]').val(config.mqtttarget.host);
                $('input[name="mqttport"]').val(parseInt(config.mqtttarget.port, 10));
                $('input[name="mqttusername"]').val(config.mqtttarget.username);
                $('input[name="mqttpassword"]').val(config.mqtttarget.password);
                $('input[name="mqtttopic').val(config.mqtttarget.topic);

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

function populateTemps(callback = null) { // Get configuration settings
    var url = "/sensors/";
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

function pollComplete() { // Poll to see if entire page is loaded
    if (loaded == numReq) {
        posted = true;
        finishPage();
    } else {
        setTimeout(pollComplete, 300); // try again in 300 milliseconds
    }
}

function finishPage() { // Display page
    toggleLoader("off");
}

// POST Handlers:

function processPost(obj) {
    posted = false;
    event.preventDefault();
    hashLoc = window.location.hash;
    var $form = $(obj);
    url = $form.attr("action");

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
        case "#targeturl":
            processTargetUrlPost(url, obj);
            break;
        case "#mqtt":
            processMQTTPost(url, obj);
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
}

function processTapPost(url, obj, tapNum) {
    // Handle tap posts

    // Get form data
    var $form = $(obj),
        ppu = $form.find("input[name='tap" + tapNum + "ppu']").val(),
        beername = $form.find("input[name='tap" + tapNum + "beername']").val(),
        cap = $form.find("input[name='tap" + tapNum + "cap']").val(),
        remain = $form.find("input[name='tap" + tapNum + "remain']").val(),
        active = $form.find("input[name='tap" + tapNum + "active']:checked").val();

    // Process post
    data = {
        tap: tapNum,
        ppu: ppu,
        beername: beername,
        cap: cap,
        remain: remain,
        active: active
    }
    postData(url, data);
}

function processControllerPost(url, obj) {
    // Handle controller posts

    // Get form data
    var $form = $(obj),
        hostnameVal = $form.find("input[name='hostname']").val(),
        brewerynameVal = $form.find("input[name='breweryname']").val(),
        kegeratornameVal = $form.find("input[name='kegeratorname']").val()
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
        // Bail out on post
        return;
    } else {
        // Process post
        toggleLoader("on");
        originalHostnameConfig = hostnameVal; // Pick up changed host name
        data = {
            hostname: hostnameVal,
            breweryname: brewerynameVal,
            kegeratorname: kegeratornameVal,
            imperial: imperialVal,
            serial: serialVal,
            tapsolenoid: tapsolenoidVal,
        }
        if (hostnamechanged && reloadpage) {
            var protocol = window.location.protocol;
            var path = window.location.pathname;
            var newpage = protocol + "//" + hostnameVal + ".local" + path + hashLoc;
            postData(url, data, newpage);
        } else if (unitschanged) {
            postData(url, data, false, true);
        } else {
            postData(url, data, false, false, function () {
                toggleLoader("off");
            });
        }
    }
}

function processTempControlPost(url, obj) {
    // Handle temperature control posts

    // Get form data
    var $form = $(obj),
        setpoint = $form.find("input[name='setpoint']").val(),
        controlpoint = $form.find("select[name='controlpoint']").val(),
        enablecontrol = $form.find("input[name='enablecontrol']:checked").val();

    // Process post
    data = {
        setpoint: setpoint,
        controlpoint: controlpoint,
        enablecontrol: enablecontrol
    }
    postData(url, data);
}

function processSensorControlPost(url, obj) {
    // Handle sensor control posts

    // Get form data
    var $form = $(obj),
        calroom = $form.find("input[name='calroom']").val(),
        enableroom = $form.find("input[name='enableroom']:checked").val(),
        caltower = $form.find("input[name='caltower']").val(),
        enabletower = $form.find("input[name='enabletower']:checked").val(),
        calupper = $form.find("input[name='calupper']").val(),
        enableupper = $form.find("input[name='enableupper']:checked").val(),
        callower = $form.find("input[name='callower']").val(),
        enablelower = $form.find("input[name='enablelower']:checked").val(),
        calkeg = $form.find("input[name='calkeg']").val(),
        enablekeg = $form.find("input[name='enablekeg']:checked").val();

    // Process post
    data = {
        calroom: calroom,
        enableroom: enableroom,
        caltower: caltower,
        enabletower: enabletower,
        calupper: calupper,
        enableupper: enableupper,
        callower: callower,
        enablelower: enablelower,
        calkeg: calkeg,
        enablekeg: enablekeg
    }
    postData(url, data, true, true);
}

function processKegScreenPost(url, obj) {
    // Handle Keg Screen Name

    // Get form data
    var $form = $(obj),
        kegscreen = $form.find("input[name='kegscreen']").val(),

        // Process post
        data = {
            kegscreen: kegscreen
        };
    postData(url, data);
}

function processTargetUrlPost(url, obj) {
    // Handle target URL posts

    // Get form data
    var $form = $(obj),
        targeturl = $form.find("input[name='targeturl']").val(),
        targetfreq = $form.find("input[name='targetfreq']").val();

    // Process post
    data = {
        targeturl: targeturl,
        targetfreq: targetfreq
    };
    postData(url, data);
}

function processMQTTPost(url, obj) {
    // Handle target URL posts

    // Get form data
    var $form = $(obj),
        mqtthost = $form.find("input[name='mqtthost']").val(),
        mqttport = $form.find("input[name='mqttport']").val();
        mqttusername = $form.find("input[name='mqttusername']").val();
        mqttpassword = $form.find("input[name='mqttpassword']").val();
        mqtttopic = $form.find("input[name='mqtttopic']").val();

    // Process post
    data = {
        mqtthost: mqtthost,
        mqttport: mqttport,
        mqttusername: mqttusername,
        mqttpassword: mqttpassword,
        mqtttopic: mqtttopic
    };
    postData(url, data);
}

function postData(url, data, newpage = false, newdata = false, callback = null) {
    var loadNew = (newpage.length > 0);
    $.ajax({
        url: url,
        type: 'POST',
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
        posted = false;
    } else {
        setTimeout(buttonClearDelay, 500); // try again in 300 milliseconds
    }
}

function updateHelp(hashLoc) {
    var url = "https://docs.kegcop.com"

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
            url = url + "/en/latest/context/settings/taps/index.html";
            break;
        case "#tempcontrol":
            url = url + "/en/latest/context/settings/temperature/control/index.html";
            break;
        case "#sensorcontrol":
            url = url + "/en/latest/context/settings/temperature/sensors/index.html";
            break;
        case "#kegscreen":
            url = url + "/en/latest/context/settings/targets/kegscreen/index.html";
            break;
        case "#targeturl":
            url = url + "/en/latest/context/settings/targets/url/index.html";
            break;
        case "#mqtt":
            url = url + "/en/latest/context/settings/targets/mqtt/index.html";
            break;
        case "#controller":
            url = url + "/en/latest/context/settings/controller/index.html";
            break;
        case "#flowcal":
            url = url + "/en/latest/context/settings/advanced/calibrate/index.html";
            break;
        default:
            // Unknown hash location passed
            break;
    }
    $("#contexthelp").prop("href", url)
}
