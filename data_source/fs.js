// Supports File Options page

toggleLoader("off");
// Semaphores
var loadFilesReasonRunning = false;
var loadFreeRunning = false;
// Pre Loader Variables
var numReq = 1 + numReqPre;
var loaded = 0;

function finishLoad() {
    // Catch page finished event from kegcop_pre.js
    chooseTempMenu();
    populatePage();
}

function populatePage() { // Get page data
    getFree();  // Populate form with controller settings
    pollComplete();
}

function finishPage() { // Display page
    // Reload here if needed
}

function getFree() {
    if (!dataHostCheckDone) {
        setTimeout(loadHeap, 10);
        return;
    }
    if (loadFreeRunning) return;
    loadFreeRunning = true;

    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/fs/fsinfo/";

    var fsinfo = $.getJSON(url, function () {
    })
        .done(function (fsinfo) {
            try {
                var free = fsinfo.f.free;
                var used = fsinfo.f.used;
                var total = fsinfo.f.total;

                $("#freespiffs").text(humanReadableSize(free));
                $("#usedspiffs").text(humanReadableSize(used));
                $("#totalspiffs").text(humanReadableSize(total));

                if (loaded < numReq) {
                    loaded++;
                }
            }
            catch {
                $('#fsinfo').text("Error parsing filesystem info.");
            }
        })
        .fail(function () {
            $('#fsinfo').text("Error loading filesystem info.");
        })
        .always(function () {
            loadFreeRunning = false;
            if (typeof callback == "function") {
                callback();
            }
        });

}

function toggleFiles() {
    if($('#fileList').is(':visible')) {
        $("#listFiles").html("List Files");
        $("#fileList").hide();
    } else {
        $("#listFiles").html("Hide Files");
        $("#fileList").show();
        listFiles();
    }
}

function listFiles(callback = null) { // Get last reset reason
    var fileHeader = $('#fileHeader');
    var fileDetails = $('#fileDetails');

    if (loadFilesReasonRunning) return;
    loadFilesReasonRunning = true;

    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/fs/listfiles/";

    var files = $.getJSON(url, function () {
    })
        .done(function (files) {
            try {
                fileHeader.html("<h5>File System Files:<h5>");
                var filesString = "<table><tr><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>"
                $.each(files, function (index, file) {
                    filesString += "<tr align='left'><td>" + file.split('|')[0] + "</td><td>" + humanReadableSize(parseInt(file.split('|')[1])) + "</td>";
                    filesString += "<td><button type=\"button\" class=\"btn btn-primary internal-action\" onclick=\"downloadDeleteButton(\'" + file.split('|')[0] + "\', \'download\')\">Download</button>";
                    filesString += "<td><button type=\"button\" class=\"btn btn-warning internal-action\"onclick=\"downloadDeleteButton(\'" + file.split('|')[0] + "\', \'delete\')\">Delete</button></tr>";
                });
                filesString += "</table>";
                fileDetails.html(filesString);
            }
            catch {
                fileDetails.html("(Error parsing file list.)");
            }
        })
        .fail(function () {
            fileDetails.html("(Error loading file list.)");
        })
        .always(function () {
            loadFilesReasonRunning = false
            if (typeof callback == "function") {
                callback();
            }
        });
}

function downloadDeleteButton(filename, action) {
    var url = "/api/v1/fs/handlefile/";
    var statusIndicator = $('#status');
    url += "?name=" + filename + "&action=" + action;
    xmlhttp = new XMLHttpRequest();
    if (action == "delete") {
        deleteFile(
            url,
            function() {
                statusIndicator.html(filename + " deleted.");
                listFiles();
                // TODO:  Timeout this message
            },
            function(errorMessage) {
                statusIndicator.html(errorMessage);
            }
        );
    }
    if (action == "download") {
        downloadFile(
            url,
            function(blob) {
                statusIndicator.html(filename + " downloaded.");
            },
            function(errorMessage) {
                statusIndicator.html(errorMessage);
            }
        );
        window.open(url, "_blank");
    }
}

function deleteFile(url, onSuccess, onFailure) {
    const xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                const blob = xhr.response;
                onSuccess();
            } else {
                onFailure(`Failed with status code ${xhr.status}`);
            }
        }
    };
    xhr.open("GET", url, true);
    xhr.send();
}

function downloadFile(url, onSuccess, onFailure) {
    const xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                const blob = xhr.response;
                onSuccess(blob);
            } else {
                onFailure(`Failed with status code ${xhr.status}`);
            }
        }
    };
    xhr.open("GET", url, true);
    xhr.responseType = "blob";
    xhr.send();
}

function showUpload() {
    $("#uploadButton").attr("disabled", true);
    $("#uploadFile").val(null);
    $("#uploadForm").show();
}

function uploadFile() {
    var fileInput = $('#uploadFile');
    var progressIndicator = $('#progressBar');
    var statusIndicator = $('#status');
    var loadedNtotal = $('#loaded_n_total');
    var fileHeader = $('#fileHeader');
    var fileDetails = $('#fileDetails');

    var file = fileInput[0].files[0];
    var fileName = file.name;

    // Call the uploadFile function with the appropriate event handlers
    doUpload(file, "/api/v1/fs/upload/", function (event) {
        // Update the progress indicator with the current progress
        var progress = Math.round((event.loaded / event.total) * 100);
        var loaded = event.loaded;
        loadedNtotal.html("Uploaded " + loaded + " bytes.");

        progressIndicator.setAttribute('aria-valuenow', progress);
        progressIndicator.setAttribute('style','width:' + Number(progress)+'%');
        progressIndicator.text(progress + '%');

        statusIndicator.html(progress + "% uploaded, please wait");
        if (progress >= 100) {
            statusIndicator.html("Please wait, writing file to filesystem.");
        }
    }, function () {
        // Handle the abort event
        statusIndicator.html("Upload Aborted.");
    }, function () {
        // Handle the complete event
        statusIndicator.html("'" + fileName + "' uploaded.");

        // progressIndicator.setAttribute('aria-valuenow', 100);
        // progressIndicator.setAttribute('style','width:' + Number(100)+'%');
        // progressIndicator.text(100 + '%');

        // xmlhttp = new XMLHttpRequest();
        listFiles();
        fileHeader.html("<h3>Files<h3>");
        // fileDetails.html(xmlhttp.responseText);

        setTimeout(function(){ cancelUpload();}, 3000);
    }, function () {
        // Handle the failure event
        statusIndicator.html("Upload Failed.");
    });
}

function doUpload(file, url, progressHandler, abortHandler, completeHandler, errorHandler, callback = null) {
    // Create a new form data object and append the file to it
    var formData = new FormData();
    formData.append('file', file);

    // Create a new XHR request
    var xhr = new XMLHttpRequest();

    // Set up the event handlers for the XHR request
    xhr.upload.addEventListener('progress', progressHandler);
    xhr.addEventListener('abort', abortHandler);
    xhr.addEventListener('load', completeHandler);
    xhr.addEventListener('error', errorHandler);

    // Open the XHR request and send the form data
    xhr.open('POST', url);
    xhr.send(formData);
}

function cancelUpload() {
    $("#uploadButton").attr("disabled", true);
    $("#uploadFile").val(null);
    $("#uploadForm").hide();
}

function fileChange() {
    if(  $('#uploadFile').get(0).files.length >= 1 ){
        $("#uploadButton").attr("disabled", false);
    } else {
        $("#uploadButton").attr("disabled", true);
    }
}

function humanReadableSize(bytes) {
    if (bytes < 1024)
        return bytes.toString() + " B";
    else if (bytes < (1024 * 1024))
        return (bytes / 1024.0).toFixed(2) + " KB";
    else if (bytes < (1024 * 1024 * 1024))
        return (bytes / 1024.0 / 1024.0).toFixed(2) + " MB";
    else
        return (bytes / 1024.0 / 1024.0 / 1024.0).toFixed(2) + " GB";
}
