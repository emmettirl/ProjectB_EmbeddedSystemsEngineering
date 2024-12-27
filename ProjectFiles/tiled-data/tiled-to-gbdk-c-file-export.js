/*
 * tiled-to-gbdk-c-file-export.js
 *
 * This extension adds the "GBDK C file" type to the "Export As" menu,
 * which generates a tile array that can be used with GBDK's background and
 * window functions such as set_bkg_tiles or set_win_tiles.
 * 
 * This extension is based on "Tiled to GBA export" by Jay van Hutten
 * https://github.com/djedditt/tiled-to-gba-export
 *
 * The first tile layer found is converted to a C array of hexadecimal tile
 * IDs casted into an unsigned 8 bit integer. Blank tiles are defaulted to 0x00.
 *
 * Copyright (c) 2020 Jay van Hutten
 * Copyright (c) 2022 Adrián Jiménez Galán
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */
function decimalToHex(p_decimal, p_padding) {
    var hexValue = (p_decimal)
        .toString(16)
        .toUpperCase()
        .padStart(p_padding, "0");

    return "0x"+hexValue;
}

var tiledToGbdkCFileExport = {
    name: "GBDK C file",
    extension: "c *.h",
    write:

    function(p_map, p_fileName) {
        console.time("Export completed in");

        // Split full filename path into the filename (without extension) and the directory
        let fileBaseName = FileInfo.completeBaseName(p_fileName).replace(/[^a-zA-Z0-9-_]/g, "_");
        let filePath = FileInfo.path(p_fileName) + "/";
        
        // Replace the ‘/’ characters in the file path for ‘\’ on Windows
        filePath = FileInfo.toNativeSeparators(filePath);

        //Generate header file data
        let headerFileData = "";
        headerFileData += "#ifndef __" + fileBaseName + "_h_INCLUDE\n";
        headerFileData += "#define __" + fileBaseName + "_h_INCLUDE\n\n";
        headerFileData += "#define " + fileBaseName +"Width " + p_map.width + "\n";
        headerFileData += "#define " + fileBaseName + "Height " + p_map.height + "\n\n";

        headerFileData += "extern const unsigned char "+ fileBaseName + "[];\n\n";
        headerFileData += "#endif\n";

        //Generate tilemap file data
        let tilemapFileData = "";
        let tilemapFileData1 = "";
        tilemapFileData += "#include \"" + fileBaseName + ".h\"\n\n";
        tilemapFileData += "#include \"Object.h\"\n\n";
        //tilemapFileData += "const unsigned char " + fileBaseName + "[] = {";

        //Find first tile layer
        let tileLayerIndex = -1;
        let currentLayer;
 // Write source data to disk
        let tilemapFile = new TextFile(filePath + fileBaseName + ".c", TextFile.WriteOnly);
        tilemapFile.write(tilemapFileData);
        tilemapFileData="";
        for (let i = 0; i < p_map.layerCount; ++i) {
            currentLayer = p_map.layerAt(i);
            if (currentLayer.isTileLayer) {
                tileLayerIndex  = i;
               // break;
            }
        
        //If no tile layer is found, throw error.
      //  if (tileLayerIndex == -1) {
        //    return "Export failed: No Tile Layer found."
       // }
 
        let ii = 0;
if (currentLayer.isTileLayer) {
   tilemapFileData += "unsigned char " +  currentLayer.name + "[] = {";
        for (let y = 0; y < currentLayer.height; y++) {
            for (let x = 0; x < currentLayer.width; x++) {
                let currentTile = currentLayer.cellAt(x, y);
                
                //Write a line break each 10 tiles
                if (ii++ % 10 == 0) {
                    tilemapFileData += "\n    ";
                }                

                //If not defined, write tile 0
                if (currentTile.tileId == -1) {
                    tilemapFileData += "0x00, ";
                } else {
                    //Cast number id into an unsigned 8 bit integer
                    let tileId = currentTile.tileId; //% 256;
                    
                    //Write tileId as hexadecimal
                    let value = ' ' + tileId;//.toString(16)//.padStart(2, "0");          
                    tilemapFileData += value + ", ";
                }
            }
        }
        // Remove the last comma and close the array.
        tilemapFileData = tilemapFileData.slice(0, -2)+"\n};\n";
        tilemapFile.write(tilemapFileData);
        tilemapFileData="";
}
else
if (currentLayer.isObjectLayer) {
     // let currentLayer1 = p_map.getObjectLayer('Collisions');
      let currentLayer1 = p_map.layerAt(i);
      tilemapFileData1="";
      let count=0;
      tilemapFileData1 += "Object * " +currentLayer.name+" = (Object[]) {\n\n"
      currentLayer1.objects.forEach(function(object) {
    //console.log(object.name, object.x, object.y, object.width, object.height);
    if ( object.shape == MapObject.Rectangle){
     tilemapFileData1 += "{   .x = "+ object.x +",\n";
      tilemapFileData1 +=" .y = "+object.y+ ",\n";
      tilemapFileData1 +=" .width = "+object.width+ ",\n";
      tilemapFileData1 +=" .height = "+object.height+ "\n},\n";
      count++;
     }
    if (object.shape == MapObject.Polygon){
       ;//tilemapFileData1 += ".polygon = (Point[]) {\n "
    }
    // You can now create game objects, use them for logic, or store them in a data structure
    // For example, create a sprite in Phaser for each object:
    //let sprite = this.add.sprite(object.x, object.y, 'spriteKey');
});
       // for (let m = 0; m < 5; m++) {
           // let obj = currentLayer1.objectAt(1);
            
            
           // }

        
         tilemapFileData1 = tilemapFileData1.slice(0, -2)+"\n};\n";
         tilemapFileData1 += "int " + currentLayer.name +"_size = "+count+";\n";
         tilemapFile.write(tilemapFileData1);
       
}
}
        // Write header data to disk
        let headerFile = new TextFile(filePath + fileBaseName + ".h", TextFile.WriteOnly);
        headerFile.write(headerFileData);
        headerFile.commit();
        console.log("Header file exported to " + filePath+fileBaseName + ".h");

       
        
        tilemapFile.commit();
        console.log("Tilemap file exported to " + filePath + fileBaseName + ".c");

        console.timeEnd("Export completed in");
    }
}

tiled.registerMapFormat("TiledToGbdkCFileExport", tiledToGbdkCFileExport)
