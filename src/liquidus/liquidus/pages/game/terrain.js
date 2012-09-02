(function () {

    var Terrain = WinJS.Class.define(function() {
        this.deform = null;
        this.type = GameEntity.Terrain;

        this.yoffset = 0;
        
    },
        {
            load: function (json) {
                var obj = JSON.parse(json)
                  , root = this._buildTree(obj)

                this.deform = new engine.TerrainCx();
                this.deform.build(root);
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
            }
        });


    WinJS.Namespace.define("App.Class",
        {
            Terrain: Terrain
        });
})();