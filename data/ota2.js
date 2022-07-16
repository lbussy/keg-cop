// Support ota2.htm

function finishLoad() { // Load the page's JS elements
    $.ajax({ // Clear any previous update flags
        url: '/api/v1/action/clearupdate/',
        type: 'PUT'
    });

    $.ajax({ // Semaphore the controller that it's safe to proceed
        url: '/api/v1/action/updatestart/',
        type: 'PUT'
    });

    // Wait for update to complete
    var running = false;
    var intervalID = window.setInterval(function () { // Poll every 5 seconds
        checkSemaphore(function (semaphore) {
            didupdate = semaphore;
            if ((didupdate == true) && (running == false)) {
                running = true;
                // Update is complete
                $.ajax({
                    url: '/api/v1/action/clearupdate/',
                    type: 'PUT'
                });
                $("#subtitle").replaceWith("<h4 class='card-header' class='card-title'>Firmware Update Complete; Redirect Pending</h4>");
                $("#message").replaceWith("<p class='card-body'>The firmware update is complete.  You will be redirected momentarily.</p>");
                setTimeout(function () { window.location.href = "index"; }, 5000);
            }
        });
    }, 10000);
}

function checkSemaphore(callback) { // Check to see if the update is complete
    var jqxhr = $.get("api/v1/action/ping")
        .done(function (data) {
            callback(true);
        })
        .fail(function () {
            // This will fail while controller resets
            callback(false);
        });
}
