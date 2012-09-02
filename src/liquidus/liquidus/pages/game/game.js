(function () {

    "use strict";

    var game = { };

    /**
     *  Ticker function
     *  The fucntion is automatically called by
     *  easeljs ticker object.
     *
     */
    function gameLoop(elapsed, ispaused) {
        
        var comps = game.components
          , i
          , cnt = comps.length
          , c
        
        for (i = 0; i < cnt; ++i) {
            c = comps[i]
            c.update && c.update(elapsed, ispaused)
        }
        for (i = 0; i < cnt; ++i) {
            c = comps[i]
            c.draw && c.draw(elapsed, ispaused)
        }
    }

    function onPageReady() {
        
        WinJS.Promise.timeout(1)
            .then(function () {
                var loader = new App.Class.WorldLoader()
                  , components = loader.load()

                game.components = components;

                createjs.Ticker.init();
                createjs.Ticker.addListener(gameLoop, true);
                createjs.Ticker.setFPS(30);
            });
    }

    WinJS.UI.Pages.define("/pages/game/game.html", {
        ready: function (element, options) {
            onPageReady();
        }
    });

})();