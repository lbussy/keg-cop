// Supports Help page

toggleLoader("off");
var numReq = 0 + numReqPre;
var loaded = 0;

function finishLoad() {
    // Catch event from kegcop_pre.js
    chooseTempMenu();
    pollComplete();
}

function finishPage() { // Display page
    // Reload here if needed
}