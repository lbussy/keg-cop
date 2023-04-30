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

function listFiles(callback = null) { // Get last reset reason
    var detailsHeader = $('#detailsHeader');
    var details = $('#details');

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
                detailsHeader.html("<h5>File System Files:<h5>");
                var filesString = "<table><tr><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>"
                $.each(files, function (index, file) {
                    filesString += "<tr align='left'><td>" + file.split('|')[0] + "</td><td>" + humanReadableSize(parseInt(file.split('|')[1])) + "</td>";
                    filesString += "<td><button type=\"button\" class=\"btn btn-primary internal-action\" onclick=\"downloadDeleteButton(\'" + file.split('|')[0] + "\', \'download\')\">Download</button>";
                    filesString += "<td><button type=\"button\" class=\"btn btn-warning internal-action\"onclick=\"downloadDeleteButton(\'" + file.split('|')[0] + "\', \'delete\')\">Delete</button></tr>";
                });
                filesString += "</table>";
                details.html(filesString);
            }
            catch {
                details.html("(Error parsing file list.)");
            }
        })
        .fail(function () {
            details.html("(Error loading file list.)");
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
    var statusIndicator = $('#status');
    var detailsHeader = $('#detailsHeader');
    var details = $('#details');
    detailsHeader.html("<h5>Upload File<h5>");
    statusIndicator.html("");
    // var uploadform =
    //     '<form method = "POST" enctype="multipart/form-data">' +
    //     '<input type="file" name="data"/>' +
    //     '<input type="submit" name="upload" value="Upload" title = "Upload File"></form>'
    // details.html(uploadform);
    var uploadform =
        '<div class="input-group">' +
        '<form id="upload_form" enctype="multipart/form-data" method="post">' +

        '<div class="input-group mb-3">' +
        '<input type="file" name="uploadfile" id="uploadfile" class="form-control" onchange="uploadFile()">' +
        '<div class="input-group-append">' +
        '<span type="button" class="btn btn-secondary internal-action" onclick="listFiles()">Cancel</span>' +
        '</div>' +
        '</div>' +

        '<br>' +

        '<div class="progress">' +
        '<div class="progress-bar progress-bar-striped progress-bar-animated" role="progressbar" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100" style="width: 75%"></div>' +
        '</div>' +

        '<h5 id="status"></h5>' +
        '<p id="loaded_n_total"></p>' +
        '</form>';
    details.html(uploadform);
}

function uploadFile() {
    var fileInput = $('#uploadfile');
    var progressIndicator = $('#progressBar');
    var statusIndicator = $('#status');
    var loadedNtotal = $('#loaded_n_total');
    var detailsHeader = $('#detailsHeader');
    var details = $('#details');

    var file = fileInput[0].files[0];
    var fileName = fileInput.val().split('/').pop().split('\\').pop();

    // Call the uploadFile function with the appropriate event handlers
    doUpload(file, "/api/v1/fs/upload/", function (event) {
        // Update the progress indicator with the current progress
        var progress = Math.round((event.loaded / event.total) * 100);
        var loaded = event.loaded;
        loadedNtotal.html("Uploaded " + loaded + " bytes.");
        progressIndicator.value(progress);
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
        statusIndicator.html("Upload Complete.");
        progressIndicator.value = 0;
        xmlhttp = new XMLHttpRequest();
        listFiles();
        statusIndicator.html(fileName + " uploaded.");
        // TODO: Timeout text
        detailsHeader.html("<h3>Files<h3>");
        details.html(xmlhttp.responseText);
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
