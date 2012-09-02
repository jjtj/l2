(function () {
    "use strict";

    var WorldLoader = WinJS.Class.define(function () {
        
    }, {
        loadAsync:function (ctx) {
            var w = World.loadPink()
              , components = [w]
              , promises = []

            ctx.world = w;
            ctx.gameCanvas.width = w.dim.w;
            ctx.gameCanvas.height = window.screen.height;
            ctx.stage = new createjs.Stage(ctx.gameCanvas)
            

            promises.push(this._loadWorldTerrainAsync(ctx))

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
        }
    });

    WinJS.Namespace.define("App.Class",
        {
            WorldLoader: WorldLoader
        });
})();