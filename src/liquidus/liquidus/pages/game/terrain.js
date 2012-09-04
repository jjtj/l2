(function () {

    var Terrain = WinJS.Class.define(function() {
        this.deform = null;
        this.type = GameEntity.Terrain;

        //this.yoffset;
        this.canvas = null
        this.canvasCtx = null
        this.ebmp = null
    },
        {
            /**
             *  Load terrain with async
             */
            loadAsync: function (ctx,
                                 screenSize,
                                 json) {

                var obj = JSON.parse(json)
                  , root = this._buildTree(obj)
                  
                this.deform = new engine.TerrainCx();
                this.deform.build(root);
                
                this._createCanvasAndBmp(screenSize[0],
                                         screenSize[1])
                
                this._renderTerrain(0)
                ctx.stage.addChild(this.ebmp)
                
                
                return WinJS.Promise.wrap(this)
            },
            
            /**
             *  Create background canvas
             */
            _createCanvasAndBmp: function (w, h) {
                var c = document.createElement("canvas")

                c.width = w
                c.height = h
                
                this.canvasCtx = c.getContext('2d')
                
                this.canvas = c
                this.ebmp = new createjs.Bitmap(this.canvas)
                this.ebmp.x = 0
                this.ebmp.y = 0
            },
            
            _buildTree: function (raw) {
                var n = new engine.QuadRawCx()
                  , children
                  , cnt
                  , i
                  , newChild
                
                n.setData(raw[0], raw[1], raw[2], raw[3], raw[4]);

                children = raw[5];
                if (children) {
                    cnt = children.length;
                    for(i=0;i<cnt;++i) {
                        newChild = this._buildTree(children[i]);
                        n.addChild(newChild);
                    }
                }

                return n;
            },
            

            /**
             *  Render terrain
             */
            _renderTerrain: function (yoffset) {
                this.yoffset = yoffset

                var t1 = new Date().getTime();

                var v = this.deform.query(0,
                    yoffset,
                    this.canvas.width,
                    this.canvas.height)
                  , cnt = v.length
                  , i
                  , x, y, w, h
                  , ctx = this.canvasCtx
                
                var diff = new Date().getTime() - t1
                console.log("time00: " + diff)
                  
                ctx.clearRect(0, 0, this.canvas.width, this.canvas.height)
                
                ctx.fillStyle = '#ff00ff'
                
                /*
                ctx.fillRect(0, 0, this.canvas.width, this.canvas.height)
                ctx.fillStyle = '#ff00ff'

                
                ctx.fillStyle = '#00f'
                ctx.fillRect(618, 1015, 7, 50);
                return;
                */
                
                console && console.log("RenderTerrain: count=" + cnt + ", numRects=" + cnt/4)
                
                t1 = new Date().getTime()

                var r, g, b;
                
                for(i=0;i<cnt;i+=4) {
                    x = v[i]
                    y = v[i + 1] - yoffset
                    w = v[i + 2]
                    h = v[i + 3]
                    
                    /* FOR VISUAL DEBUGGING
                     * ====================
                    r = Math.floor(Math.random() * 45731) % 256
                    g = Math.floor(Math.random() * 93411) % 256
                    b = Math.floor(Math.random() * 17231) % 256
                    if (r < 16) r += 16;
                    if (g < 16) g += 16;
                    if (b < 16) b += 16;
                    
                    ctx.fillStyle = '#' + r.toString(16)
                                    + g.toString(16)
                                    + b.toString(16)
                    
                    console.log('x:' + x + ', y:' + y + ' w:' + w+ ', h:' + h)
                    */
                    
                    ctx.fillRect(x,y,w,h)
                }
                
                diff = new Date().getTime() - t1
                console.log("timeXX: " + diff)
            },
            

            /**
             *  Update y-offset
             *
             */
            updateYOffset: function (newYOffset) {
                this._renderTerrain(newYOffset)
            }
        });


    WinJS.Namespace.define("App.Class",
        {
            Terrain: Terrain
        });
})();