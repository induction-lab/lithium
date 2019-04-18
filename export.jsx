#target photoshop
  
var doc = activeDocument;
var docPath = doc.path;
var XY = '';  
var txtDoc = new File(doc.path + '/' + doc.name.split('.')[0]+'.txt');
doc.activeLayer = doc.layers[0];
getXY(doc);
writeFile(txtDoc, XY);
  
function getXY(lSet) {
    for (var i = 0; i < lSet.layers.length; i++) {
        doc.activeLayer = lSet.layers[i];
        if (doc.activeLayer.typename == 'LayerSet') {
            if (doc.activeLayer.layers.length > 0) {
                getXY (doc.activeLayer)
            }
        } else {
            // Format the following line how you want your text document.
            var X = doc.activeLayer.bounds[0].toString().replace(' px', '');
            var Y = doc.activeLayer.bounds[1].toString().replace(' px', '');
            XY += X + ' ' + Y + '\n';
        };
    };
};

function writeFile(file, txt) {
    file.encoding = "UTF8";
    file.open("w", "TEXT", "????");
    /*
    // unicode signature, this is UTF16 but will convert to UTF8 "EF BB BF"
    file.write("\uFEFF");
    */
    file.lineFeed = "unix";
    file.write(txt);
    file.close();
};