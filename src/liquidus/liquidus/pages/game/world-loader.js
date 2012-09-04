(function () {
    "use strict";

    var WorldLoader = WinJS.Class.define(function () {
        
    }, {
        loadAsync:function (ctx) {
            var w = World.loadPink()
              , components = [w]
              , promises = []
              , lvl

            ctx.world = w;
            ctx.gameCanvas.width = w.dim.w;
            ctx.gameCanvas.height = window.screen.height;
            ctx.stage = new createjs.Stage(ctx.gameCanvas)
            
            promises.push(this._loadWorldTerrainAsync(ctx))
            
            for (var x in w.level) {
                lvl = w.level[x]
                this._loadLevelAsync(ctx, lvl)
            }

            return WinJS.Promise.thenEach(promises,
                function(v) {
                    components.push(v);
                })
                .then(function () {
                    return WinJS.Promise.wrap(components);
                });
        },
        
        /**
         * Load world terrain
         * 
         * @param w {World} World data
         *
         */
        _loadWorldTerrainAsync: function (ctx) {
            var ter = new App.Class.Terrain()
              , w = ctx.world
              , json = w.wolrdTerrain
              , canvasSize = [w.dim.w,
                             window.screen.height]
            
            return ter.loadAsync(ctx, canvasSize, json)
        },

        /** 
         *  Load level async
         *
         *  @param ctx {Context} Context object
         *  @param data {Object} Level data
         *              checkPoints - array of check point
         *                  checkPoint - x,y,w,h,id,imgfile,isLine
         *              levelNum - Level number
         *              terrain - Level's terrain boundary
         *              title - Level's title
         *                  id,x,y,w,h,imgfile
         *
         */
        _loadLevelAsync: function (ctx, promises, data) {
        }
    });

    WinJS.Namespace.define("App.Class",
        {
            WorldLoader: WorldLoader
        });
})();