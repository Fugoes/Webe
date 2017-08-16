function ToggleBetweenDisplayAndHide(obj) {
    var elem = document.getElementById(obj);
    if (elem.className === "hide") {
        elem.className = "display";
    } else {
        elem.className = "hide";
    }
}
