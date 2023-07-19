// Supports License page

toggleLoader("off");
var numReq = 1 + numReqPre;
var loaded = 0;

function finishLoad() { // Get page data
    chooseTempMenu();
    pollComplete();
}

function finishPage() { // Display page
    // Reload here if needed
}
