(function () {
    "use strict";

    var ImageEntity = WinJS.Class.define(function () {

        this.imgpath = null;
        this.img = null;
        this.isFixed = true;
        this.ebmp = null;
        this.xOrigin = 0;
        this.yOrigin = 0;
    },
    {
        /**
         *  Load async image entity
         */
        loadAsync: function (ctx,
                             x, y,
                             imgpath,
                             isFixed) {
            this.imgpath = imgpath;
            this.isFixed = isFixed;
            this.xOrigin = x;
            this.yOrigin = y;

            Utils.loadImageAsync(imgpath)
                 .then(function (evt) {
                     this.img = evt.img;
                     this.ebmp = new createjs.Bitmap(this.img);
                     this.ebmp.x = x;
                     this.ebmp.y = y;

                     ctx.stage.addChild(this.ebmp);
                 });
        },


        /**
         *  Update y-offset
         *
         *  @param newYOffset {Number} new global y-offset.
         *
         */
        updateYOffset: function (newYOffset) {
            if (this.isFixed)
                return;

            this.ebmp.y = this.yOrigin + newYOffset;
        }
    });


    WinJS.Namespace.define("App.Class",
        {
            ImageEntity: ImageEntity
        });

})();