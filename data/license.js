// Supports License page

toggleLoader("off");
var numReq = 1;
var loaded = 0;

function finishLoad() { // Get page data
    populateTemps();
    pollComplete();
}

function finishPage() { // Display page
    // Reload here if needed
}
