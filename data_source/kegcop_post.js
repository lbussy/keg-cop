// Common functions requiring jQuery

// Turn off tooltips on radio button change {" "}
$('input[type=radio]').change(function () {
    $('[data-toggle="tooltip"], .tooltip').tooltip("hide");
});

// AJAX errors
// Used in index, settings, temps

tempAlert = function () { }
tempAlert.warning = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-warning" id="warnTempError">'
        _div += '<button type="button" class="btn-close" data-dismiss="alert"></button>'
        _div += '<p class="mb-0"><strong>Temps Warning: </strong><span>' + message + '</span></p></div>'
    }
    $('#flowAlert_placeholder').html(_div);
}

configAlert = function () { }
configAlert.warning = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-warning" id="warnConfigError">'
        _div += '<button type="button" class="btn-close" data-dismiss="alert"></button>'
        _div += '<p class="mb-0"><strong>Config Warning: </strong><span>' + message + '</span></p></div>'
    }
    $('#configAlert_placeholder').html(_div);
}

flowAlert = function () { }
flowAlert.warning = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-warning" id="warnFlowError">'
        _div += '<button type="button" class="btn-close" data-dismiss="alert"></button>'
        _div += '<p class="mb-0"><strong>Flow Warning: </strong><span>' + message + '</span></p></div>'
    }
    $('#flowAlert_placeholder').html(_div);
}

settingsAlert = function () { }
settingsAlert.error = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-danger" id="warnSettingsError">'
        _div += '<button type="button" class="btn-close" data-dismiss="alert"></button>'
        _div += '<p class="mb-0"><strong>Settings Error: </strong><span>' + message + '</span></p></div>'
    }
    $('#settingsAlert_placeholder').html(_div);
}

function tempStatus(temps) { // Set temperature and fan status
    if (temps.tfancontrolenabled) {
        // Set tower fan indicator
        $("#fanstatetooltip").show();
        switch (temps.tfanstate) {
            case 0: // TSTAT_INACTIVE
                $('#towerFan').html('<i class="fa-solid fa-fan"></i>');
                $("#fanstatetooltip").attr("data-original-title", "Tower fan is off.");
                break;
            case 1: // TSTAT_COOL_BEGIN
                $('#towerFan').html('<i class="fa-solid fa-spin fa-fan"></i>');
                $("#fanstatetooltip").attr("data-original-title", "Tower fan is on.");
                break;
            case 2: // TSTAT_COOL_MINOFF
                $('#towerFan').html('<i class="fa-solid fa-fan"></i>');
                $("#fanstatetooltip").attr("data-original-title", "Tower fan is off.");
                break;
            case 3: // TSTAT_COOL_ACTIVE
                $('#towerFan').html('<i class="fa-solid fa-spin fa-fan"></i>');
                $("#fanstatetooltip").attr("data-original-title", "Tower fan is on.");
                break;
            case 4: // TSTAT_IDLE_END
                $('#towerFan').html('<i class="fa-solid fa-fan"></i>');
                $("#fanstatetooltip").attr("data-original-title", "Tower fan is off.");
                break;
            case 5: // TSTAT_IDLE_MINON
                $('#towerFan').html('<i class="fa-solid fa-spin fa-fan"></i>');
                $("#fanstatetooltip").attr("data-original-title", "Tower fan is on.");
                break;
            case 6: // TSTAT_IDLE_INACTIVE
                $('#towerFan').html('<i class="fa-solid fa-fan"></i>');
                $("#fanstatetooltip").attr("data-original-title", "Tower fan is off.");
                break;
            case 7: // TSTAT_UNKNOWN
                $('#towerFan').html('<i class="fa-solid fa-fan"></i>');
                $("#fanstatetooltip").attr("data-original-title", "Tower fan state unknown.");
                break;
            default: // TSTAT_UNKNOWN
                $('#towerFan').html('<i class="fa-solid fa-fan"></i>');
                $("#fanstatetooltip").attr("data-original-title", "Tower fan state unknown.");
                break;
        }
    } else {
        $("#fanstatetooltip").hide();
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
