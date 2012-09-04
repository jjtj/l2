(function () {
    "use strict";

    var LevelEntity = WinJS.Class.define(function () {
        this.levelNum = 0;
        this.title = null;
        this.checkPoints = null;
        this.x = 0;
        this.y = 0;
        this.w = 0;
        this.h = 0;
    },
    {
        /**
         *  Load level object in async.
         *
         */
        loadAsync: function (ctx,
                            lvlData) {

            this.title = new 

        }
    });

    WinJS.Namespace.define("App.Class",
        {
            LevelEntity: LevelEntity
        });
})();