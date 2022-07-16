// Support wifireset.htm

function finishLoad() {
    // This actuallt resets wifi settings, after
    // this it's all over.  No need to try anything
    // else
    $.ajax({
        url: '/api/v1/action/wifireset/',
        type: 'PUT'
    });
}
