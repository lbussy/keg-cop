// Supports File Options page

toggleLoader("off");
// Semaphores
var listFilesRunning = false;
var loadFreeRunning = false;
var downloadFileRunning = false;
var deleteFileRunning = false;
// Pre Loader Variables
var numReq = 1 + numReqPre;
var loaded = 0;

function finishLoad() {
    // Catch page finished event from kegcop_pre.js
    chooseTempMenu();
    populatePage();
    eventListeners();
}

function eventListeners() {
    $("#listFiles").on("click", function (event) {
        event.preventDefault();
        toggleFiles();
    });
    $("#uploadFile").on("click", function (event) {
        event.preventDefault();
        showUpload();
    });
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
        setTimeout(getFree, 10);
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

function toggleFiles(event) {
    if ($('#fileList').is(':visible')) {
        $("#listFiles").html("List Files");
        $("#fileList").hide();
    } else {
        $("#listFiles").html("Hide Files");
        $("#fileList").show();
        listFiles();
    }
}

function listFiles(callback = null) {
    if (!dataHostCheckDone) {
        setTimeout(listFiles, 10);
        return;
    }
    if (listFilesRunning) return;
    listFilesRunning = true;

    $(':button').attr('disabled', 'disabled');

    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/fs/listfiles/";

    var fileHeader = $('#fileHeader');
    var fileDetails = $('#fileDetails');

    $.ajax({
        url: url,
        dataType: "json"
    })
        .done(function (files) {
            try {
                fileHeader.html('<h5>File System Files:<h5>');
                var filesString = '<table class="table table-hover"><tr><th scope="col" align="left">Name</th><th scope="col" align="left">Size</th><th scope="col">Action</th></tr>';
                $.each(files, function (index, file) {
                    var fileSize = humanReadableSize(parseInt(file.split('|')[1]));
                    var fileName = file.split('|')[0].trim();
                    filesString += '<tr align="left"><td>' + fileName + '</td><td>' + fileSize + '</td>';
                    filesString += '<td><button type="button" class="btn btn-primary internal-action" onclick="downloadFile(\'' + fileName + '\')">Download</button>';
                    filesString += '&nbsp;';
                    filesString += '<button type="button" class="btn btn-warning internal-action" onclick="deleteFile(\'' + fileName + '\')">Delete</button></td></tr>';
                });
                filesString += '</table>';
                fileDetails.html(filesString);
            }
            catch {
                fileDetails.html('(Error parsing file list.)');
            }
        })
        .fail(function () {
            fileDetails.html('(Error loading file list.)');
        })
        .always(function () {
            listFilesRunning = false
            if (typeof callback == "function") {
                callback();
            }
            $(':button').removeAttr("disabled");
        });
}

function downloadFile(filename) {
    if (!dataHostCheckDone) {
        setTimeout(downloadFile(filename), 10);
        return;
    }
    if (downloadFileRunning) return;
    downloadFileRunning = true;

    $(':button').attr('disabled', 'disabled');

    var fileStatus = $('#fileStatus');
    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/fs/handlefile/";
    url += "?name=" + filename + "&action=download";

    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.responseType = 'blob'; // Set the response type to blob

    xhr.onload = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                const blob = xhr.response;
                window.open(url, "_blank");
            } else {
                fileStatus.html('Request failed. Status: ' + xhr.status);
                setTimeout(function () { fileStatus.html(""); }, 3000);
            }
            $(':button').removeAttr("disabled");
        }
    };
    xhr.onerror = function () {
        fileStatus.html('Request failed. Network error.');
        setTimeout(function () { fileStatus.html(""); }, 3000);
    };
    xhr.onabort = function () {
        fileStatus.html('Request aborted.');
        setTimeout(function () { fileStatus.html(""); }, 3000);
    };
    xhr.onloadend = function () {
        fileStatus.html('Request completed.');
        setTimeout(function () { fileStatus.html(""); }, 3000);
    };
    xhr.send();
}

function deleteFile(filename) {
    if (!dataHostCheckDone) {
        setTimeout(deleteFile, 10);
        return;
    }
    if (deleteFileRunning) return;
    deleteFileRunning = true;

    $(':button').attr('disabled', 'disabled');

    if (!window.confirm("Are you sure?")) {
        $(':button').removeAttr("disabled");
        deleteFileRunning = false;
        return false;
    }

    var fileStatus = $('#fileStatus');

    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/fs/handlefile/?name=" + filename + "&action=delete";

    $.ajax({
        type: "POST",
        url: url,
        headers: { "X-KegCop-Secret": secret },
        data: { file_name: filename }
    })
        .done(function () {
            fileStatus.html(filename + " deleted.");
            listFiles();
            setTimeout(function () { fileStatus.html(""); }, 3000);
        })
        .fail(function (textStatus) {
            fileStatus.html(textStatus);
            setTimeout(function () { fileStatus.html(""); }, 3000);
        })
        .always(function () {
            deleteFileRunning = false;
            $(':button').removeAttr("disabled");
        })
        .then(function () {
            //
        });
}

function showUpload() {
    //$("#uploadButton").attr("disabled", true); // DEBUG
    $("#uploadFile").val(null);
    $("#uploadForm").show();
    $("#uploadForm").html("Sorry, upload is currently broken."); // DEBUG
    setTimeout(function () { cancelUpload(); }, 3000); // DEBUG
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
        progressIndicator.setAttribute('style', 'width:' + Number(progress) + '%');
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

        setTimeout(function () { cancelUpload(); }, 3000);
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
    if ($('#uploadFile').get(0).files.length >= 1) {
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
