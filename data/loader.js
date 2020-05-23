// Support page load spinner

function toggleLoader(status) {
    var loader = document.getElementById("loader");
    var tempsApp = document.getElementById("tempsApp");
    var settingsApp = document.getElementById("settingsApp");
    var indexApp = document.getElementById("indexApp");
    if (status === "on") {
        if (loader) {
            loader.style.display = "block";
        }
        if (tempsApp) {
            tempsApp.style.visibility = "hidden";
        }
        if (settingsApp) {
            settingsApp.style.visibility = "hidden";
        }
        if (indexApp) {
            indexApp.style.visibility = "hidden";
        }
    } else {
        if (loader) {
            loader.style.display = "none";
        }
        if (tempsApp) {
            tempsApp.style.visibility = "visible";
        }
        if (settingsApp) {
            settingsApp.style.visibility = "visible";
        }
        if (indexApp) {
            indexApp.style.visibility = "visible";
        }
    }
}
