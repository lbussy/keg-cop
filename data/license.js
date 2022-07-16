// Supports License page

toggleLoader("off");
var numReq = 1;
var loaded = 0;

function finishLoad() { // Get page data
    populateTemps();
    pollComplete();
}

function pollComplete() {
    if (loaded == numReq) {
        finishPage();
    } else {
        setTimeout(pollComplete, 300); // try again in 300 milliseconds
    }
}

function finishPage() { // Display page
    // Reload here if needed
}
