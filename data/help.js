// Supports Index page

toggleLoader("off");
var numReq = 1;
var loaded = 0;

function finishLoad() {
    // Catch event from kegcop.js
    populateTemps();
    pollComplete();
}

function finishPage() { // Display page
    // Reload here if needed
}
