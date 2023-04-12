// Supports About Page

//toggleLoader("off");
toggleLoader("on");

// Pre Loader Variables
var numReq = 0 + numReqPre;
var loaded = 0;
// Page Variables
var posted = false;

function finishLoad() { // Get page data
    fixEmbeds()
    chooseTempMenu();
    pollComplete();
}

function processPost(obj) {
    if (this.document.activeElement.getAttribute("id") == "clear")
        return false; // Clear form

    posted = false;
    var $form = $(obj);
    var actionURL = $form.attr("action");

    var textAreaID = $form.find('textarea').first().attr('id');
    var field = document.getElementById(textAreaID);
    var bulkSettings;

    try {
        bulkSettings = JSON.parse(field.value);
    }
    catch (error) {
        if (error instanceof SyntaxError) {
            alert("There was a syntax error in your JSON. Please correct it and try again:\n\n" + error.message);
        }
        else {
            throw error;
        }
        return false;
    }
    if (this.document.activeElement.getAttribute("id") == "prettyPrint") {
        // Do pretty print
        var checkJSON = prettyPrint(bulkSettings)
        if (checkJSON) {
            field.value = checkJSON;
        }
        return false;
    }

    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += actionURL;

    $(':submit').prop('disabled', true);
    $(':submit').html('<i class="fa fa-spinner fa-spin"></i> Updating');

    var header;
    if (String(textAreaID) == "appConfigBulkText") {
        header = "AppConfig"; 
    } else if (textAreaID == "flowConfigBulkText") {
        header = "FlowConfig";
    }
    putConfigData(url, bulkSettings, header);

    buttonClearDelay();
    return false; // Prevent default event
}

function finishPage() { // Display page
    toggleLoader("off");
}

// function prettyPrint(json) {
//     if (typeof json != 'string') {
//         json = JSON.stringify(json, undefined, 4);
//         return json;
//     }
//     return false
// }

// function readFile(obj) {
//     var app = obj.parentNode.id
//     var file = obj.files[0];
//     var reader = new FileReader();

//     reader.readAsText(file);
//     reader.onload = function () {
//         switch (app) {
//             case "appConfig":
//                 document.getElementById("appConfigBulkText").value = reader.result;
//                 break;
//             case "flowConfig":
//                 document.getElementById("flowConfigBulkText").value = reader.result;
//                 break;
//             default:
//                 break;
//         }
//     };

//     reader.onerror = function () {
//         alert(reader.error);
//     };
// }

// function clearFile(obj) {
//     var app = obj.parentNode.id
//     obj.form.reset();
//     switch (app) {
//         case "appConfig":
//             document.getElementById("appConfigBulkText").value = "";
//             break;
//         case "flowConfig":
//             document.getElementById("flowConfigBulkText").value = "";
//             break;
//         default:
//     }
// }

// function putConfigData(url, json, header, callback = null) {
//     const secretData = {
//         secret: secret
//     };
//     const jsonPacket = {
//         data: json
//     }
//     const data = Object.assign(secretData, jsonPacket);

//     $.ajax({
//         url: url,
//         headers: {
//             "X-KegCop-BulkLoad-Type"   : header,
//             "X-Secret"          : secret
//         },
//         data: data,
//         type: 'PUT'
//     })
//         .done(function (data) {
//             settingsAlert.error();
//         })
//         .fail(function (data) {
//             settingsAlert.error("Settings update failed.");
//         })
//         .always(function (data) {
//             posted = true;
//             if (typeof callback == "function") {
//                 callback();
//             }
//         });
// }

function fixEmbeds()
{
    var editPage = cleanURL("edit");
    console.log(editPage);
    return;
    // $('#editbox').attr("src", "")
    // $('#editbox').height = "";
    //$('#editbox').height = $('#editbox').contentWindow.document.body.scrollHeight + "px";
}
