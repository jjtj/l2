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

            var t = lvlData.title

            this.title = new App.Class.ImageEntity();
            return this.loadAsync(ctx,
                           t.x, t.y,
                           t.imgpath,
                           false);
        }
    });

    WinJS.Namespace.define("App.Class",
        {
            LevelEntity: LevelEntity
        });
})();