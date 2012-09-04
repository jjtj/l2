(function () {
    "use strict";

    var Utils = {
        /**
         *  Load image by async
         */
        loadImageAsync: function (url, userData) {
            return new WinJS.Promise(
                function (c, e, p) {
                    var img = new Image();
                    img.onload = function () { c({ img: img, userData: userData }); };
                    img.onerror = function () { e('failed to load image:' + url); };
                    img.src = url;
                });
        }
    };


    WinJS.Namespace.define("Utils",
        Utils);
})();