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

// Get a reference to the progress bar, wrapper & status
var progress = document.getElementById("progress");

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
    $("#controlUpload").on("click", function (event) {
        event.preventDefault();
        toggleUpload();
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
        // Hide the Upload
        $("#fileUpload").hide();
        $("#controlUpload").text("Show Upload");
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

    $.ajax({
        url: url,
        dataType: "json"
    })
        .done(function (files) {
            try {
                $('#fileHeader').html('<h5>Filesystem Files:<h5>');
                var filesString = '<table class="table table-hover"><tr><th scope="col" align="left">Name</th><th scope="col" align="left">Size</th><th scope="col">Action</th></tr>';
                $.each(files, function (index, file) {
                    var fileSize = humanReadableSize(parseInt(file.split('|')[1]));
                    var fileName = file.split('|')[0].trim();
                    filesString += '<tr align="left"><td>' + fileName + '</td><td>' + fileSize + '</td>';
                    filesString += '<td>'
                    filesString += '<button type="button" class="btn btn-primary internal-action" onclick="downloadFile(\'' + fileName + '\')">Download</button>';
                    filesString += '&nbsp;';
                    filesString += '<button type="button" class="btn btn-warning internal-action" onclick="deleteFile(\'' + fileName + '\')">Delete</button></td></tr>';
                });
                filesString += '</table>';
                $('#fileDetails').html(filesString);
            }
            catch {
                $('#fileDetails').html('(Error parsing file list.)');
            }
        })
        .fail(function () {
            $('#fileDetails').html('(Error loading file list.)');
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

    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/fs/downloadfile/";
    url += "?name=" + filename;

    var xhr = new XMLHttpRequest();
    xhr.open('GET', url);
    xhr.responseType = 'blob'; // Set the response type to blob

    xhr.onload = function () {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                const blob = xhr.response;
                window.open(url, "_blank");
            } else {
                $('#fileStatus').html('Request failed. Status: ' + xhr.status);
                setTimeout(function () { $('#fileStatus').html(""); }, 3000);
            }
        }
    };
    xhr.onerror = function () {
        $('#fileStatus').html('Request failed. Network error.');
        setTimeout(function () { $('#fileStatus').html(""); }, 3000);
    };
    xhr.onabort = function () {
        $('#fileStatus').html('Request aborted.');
        setTimeout(function () { $('#fileStatus').html(""); }, 3000);
    };
    xhr.onloadend = function () {
        $('#fileStatus').html('Request completed.');
        setTimeout(function () { $('#fileStatus').html(""); }, 3000);
        $(':button').removeAttr("disabled");
        downloadFileRunning = false;
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

    if (!window.confirm(`Delete: ${filename}\n\nAre you sure?`)) {
        $(':button').removeAttr("disabled");
        deleteFileRunning = false;
        return false;
    }

    var url = dataHost;
    if (url && url.endsWith("/")) {
        url = url.slice(0, -1)
    }
    url += "/api/v1/fs/deletefile/";

    data = {
        delete: filename,
    };

    $.ajax({
        type: "PUT",
        url: url,
        headers: { "X-KegCop-Secret": secret },
        data,
    })
        .done(function () {
            $('#fileStatus').html(filename + " deleted.");
            listFiles();
            setTimeout(function () { $('#fileStatus').html(""); }, 3000);
        })
        .fail(function (textStatus) {
            $('#fileStatus').html(textStatus);
            setTimeout(function () { $('#fileStatus').html(""); }, 3000);
        })
        .always(function () {
            deleteFileRunning = false;
            $(':button').removeAttr("disabled");
        })
        .then(function () {
            //
        });
}

function toggleUpload() {
    if ($("#fileUpload").is(":visible")) {
        $("#fileUpload").hide();
        $("#controlUpload").text("Show Upload");
    } else {
        $("#fileUpload").show();
        $("#controlUpload").text("Hide Upload");
        // Hide the File List
        $("#listFiles").html("List Files");
        $("#fileList").hide();
    }
}

function upload() { // Function to upload file

    url = "/api/v1/fs/upload/";

    // Reject if the file input is empty & throw alert
    if (!$("#file_input").val()) {
        $("#progress_status").text("No file selected.");
        return;
    }

    // Create a new FormData instance
    var data = new FormData();
    // Create a XMLHTTPRequest instance
    var request = new XMLHttpRequest();
    // Set the response type
    request.responseType = "text";

    // Disable the input during upload
    $("#file_input").attr("disabled", "disabled");
    // Hide the upload button
    $("#upload_btn").addClass("d-none");
    // Show the loading button
    $("#loading_btn").removeClass("d-none");
    // Show the cancel button
    $("#cancel_btn").removeClass("d-none");
    // Show the progress bar
    $("#progress_wrapper").removeClass("d-none");
    // Get a reference to the file
    var file = $("#file_input").prop('files')[0];
    // Get a reference to the filename
    var filename = file.name;
    // Get a reference to the filesize & set a cookie
    var filesize = file.size;
    // Append the file to the FormData instance
    data.append("file", file);
    // Set status to waiting
    $("#progress_status").text("Waiting.");

    // Event Handlers:
    //
    // Request progress handler
    request.upload.addEventListener("progress", function (e) {
        // Get the loaded amount and total filesize (bytes)
        var loaded = e.loaded;
        var total = e.total

        // Calculate percent uploaded
        var percent_complete = (loaded / total) * 100;

        // Update the progress text and progress bar
        progress.setAttribute("style", `width: ${Math.floor(percent_complete)}%`);
        $("#progress_status").text(`${Math.floor(percent_complete)}% uploaded.`);
    });

    // Request load handler (transfer complete success)
    request.upload.addEventListener("load", function (e) {
        uploadComplete();
        $("#progress_status").text(`${filename} uploaded.`);
    });

    // Request error handler
    request.addEventListener("error", function (e) {
        uploadComplete();
        $("#progress_status").text(`Error uploading ${filename}`);
    });

    // Request abort handler
    request.addEventListener("abort", function (e) {
        uploadComplete();
        $("#progress_status").text(`Upload of ${filename} cancelled`);
    });

    // Cancel event listener
    $("#cancel_btn").on("click", function () {
        request.abort();
    })

    // Open and send the request
    request.open("POST", url);
    // Add Secret
    request.setRequestHeader("X-KegCop-Secret", secret);
    request.send(data);
}

function input_filename() {// Function to update the input placeholder
    // Hide the progress bar
    $("#progress_wrapper").addClass("d-none");
    // Reset the progress bar state
    progress.setAttribute("style", `width: 0%`);
    // Reset progress text
    $("#progress_status").text('');
    // Set input to file name
    $("#file_input_label").text($("#file_input").prop('files')[0].name);
    // Enable upload button
    $("#upload_btn").attr("disabled", false);
}

function uploadComplete() { // Function to show upload complete
    // Clear the input
    $("#file_input").val('');;
    // Hide the cancel button
    $("#cancel_btn").addClass("d-none");
    // Reset the input element
    $("#file_input").removeAttr('disabled');
    // Show the upload button
    $("#upload_btn").removeClass("d-none");
    // Hide the loading button
    $("#loading_btn").addClass("d-none");
    // Reset the input placeholder
    $("#file_input_label").text("Select file");
    // Disable input button
    $("#upload_btn").attr("disabled", "disabled");
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
