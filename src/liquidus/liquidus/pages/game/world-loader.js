(function () {
    "use strict";

    var WorldLoader = WinJS.Class.define(function () {
        
    }, {
        load:function () {
            var w = World.loadPink()
              , components = [w]

            components.push(this._createWorldTerrain(w));
            
            return components;
        },
        
        /**
         * Create world terrain
         * 
         * @param w {World} World data
         *
         */
        _createWorldTerrain: function (w) {
            var ter = new App.Class.Terrain();

            ter.load(w.wolrdTerrain);
            return ter;
        }
    });

    WinJS.Namespace.define("App.Class",
        {
            WorldLoader: WorldLoader
        });
})();