// Support OTA2 page

toggleLoader("off");
checkDataHost();    // Check if we are using a dataHost

function finishLoad() { // Load the page's JS elements
    // populateTemps(); // We don't need this but it's here so I rememnber not to add it again
    if (!dataHostCheckDone) {
        setTimeout(finishLoad, 10);
        return;
    }
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
