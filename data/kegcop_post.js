// Common functions requiring jQuery

// Attach the event after the page loads
if (window.addEventListener) window.addEventListener("load", preLoad, false);
else if (window.attachEvent) window.attachEvent("onload", preLoad);
else window.onload = preLoad;

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
        _div += '<button type="button" class="close" data-dismiss="alert">&times;</button><p class="mb-0">'
        _div += '<strong>Temps Warning: </strong><span>' + message + '</span></p></div>'
    }
    $('#flowAlert_placeholder').html(_div);
}

configAlert = function () { }
configAlert.warning = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-warning" id="warnConfigError">'
        _div += '<button type="button" class="close" data-dismiss="alert">&times;</button><p class="mb-0">'
        _div += '<strong>Config Warning: </strong><span>' + message + '</span></p></div>'
    }
    $('#flowAlert_placeholder').html(_div);
}

flowAlert = function () { }
flowAlert.warning = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-warning" id="warnFlowError">'
        _div += '<button type="button" class="close" data-dismiss="alert">&times;</button><p class="mb-0">'
        _div += '<strong>Flow Warning: </strong><span>' + message + '</span></p></div>'
    }
    $('#flowAlert_placeholder').html(_div);
}

settingsAlert = function () { }
settingsAlert.error = function (message) {
    _div = "";
    if (message) {
        _div = '<div class="alert alert-dismissible alert-danger" id="warnSettingsError">'
        _div += '<button type="button" class="close" data-dismiss="alert">&times;</button><p class="mb-0">'
        _div += '<strong>Settings Error: </strong><span>' + message + '</span></p></div>'
    }
    $('#settingsAlert_placeholder').html(_div);
}
