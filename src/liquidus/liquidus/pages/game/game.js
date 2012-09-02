(function () {

    "use strict";

    function GameVm() {
        var self = this;

        self.isLoaded = ko.observable(false);
        self.components = []
        self.ctx = null
        
        self.isMouseDown = false
        self.xDown = 0
        self.yDown = 0
        self.yDownOffset = 0
        
        self.yOffset = 0

        /**
         * launch game
         */
        self.launch = function (ctx) {
            self.ctx = ctx
            
            WinJS.Promise.timeout(1)
            .then(function () {
                var loader = new App.Class.WorldLoader()
                return loader.loadAsync(ctx)
            })
            .then(function (components) {
                self.components = components;
                
                self.isLoaded(true)
                
                return WinJS.Promise.timeout(1);
            })
            .then(function () {
                createjs.Ticker.init();
                createjs.Ticker.addListener(self, true);
                createjs.Ticker.setFPS(30);
            });
        };
        
        /**
         *  Ticker function
         *  The fucntion is automatically called by
         *  easeljs ticker object.
         *
         */
        self.tick = function (elapsed, ispaused) {
            var comps = self.components
              , i
              , cnt = comps.length
              , c

                self.ctx.stage.update()

                for (i = 0; i < cnt; ++i) {
                    c = comps[i]
                    c.update && c.update(elapsed, ispaused)
                }
                for (i = 0; i < cnt; ++i) {
                    c = comps[i]
                    c.draw && c.draw(elapsed, ispaused)
                }
        };
     
        /**
         *  Mouse down
         */
        self.mouseDown = function (vm, evt) {
            evt.preventDefault()
            
            self.isMouseDown = (evt.pointerType === 4 || evt.pointerType === 2)
                        && (evt.button === 0)
            if(false === self.isMouseDown)
                return
            
            self.xDown = evt.clientX
            self.yDown = evt.clientY
            self.yDownOffset = self.yOffset

        }
        
        self.mouseMove = function (vm, evt) {
            evt.preventDefault()
            if(!self.isMouseDown)
                return
            
            var dy = self.yDown - evt.clientY
              , newOffset = self.yDownOffset + dy
            
            self.updateYOffset(newOffset)
        }
        

        self.mouseUp = function (vm, evt) {
            evt.preventDefault()
            
            self.isMouseDown = false
        }
        
        
        
        self.mouseCancel = function (vm, evt) {
            evt.preventDefault()
            self.isMouseDown = false
        }
        
        /**
         *  Update y offset
         */
        self.updateYOffset = function (yoffset) {
            
            if (!self.components)
                return
            
            self.yOffset = yoffset
            
            var cnt = self.components.length
              , i
              , c
            
            for (i = 0; i < cnt; ++i) {
                c = self.components[i]
                c.updateYOffset && c.updateYOffset(yoffset)
            }
        }
    }

    /**
     *  GamePage is ready
     *
     */
    function onPageReady() {
        var gameVm = new GameVm()
          , el = document.getElementById('_gamescreen')
          , ctx = {
              gameCanvas: _gameCanvas,
              stage: null
          }
        
        ko.applyBindings(gameVm, el)
        gameVm.launch(ctx)
    }

    WinJS.UI.Pages.define("/pages/game/game.html", {
        ready: function (element, options) {
            onPageReady();
        }
    });

})();