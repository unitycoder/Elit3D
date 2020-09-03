#version 330 core

in vec2 TexCoord;

uniform sampler2D tileAtlas;
uniform sampler2D tilemap;

uniform ivec2 ntilesMap;
uniform ivec2 ntilesAtlas;

out vec4 FragColor;

void main()
{   
    //Get the pixel information from the texture of the map that indicates de column and the row of the tileset
    vec4 colRow = texture(tilemap, TexCoord) * 255.0; // * 255 because it returns a normalized value

    /*
    I use z coordinate as a helper:
        * 0 means there isn't a tile
        * other number is a multiplier for long tilesets (ntilesAtlas > 255 tiles)
    */  
    if(colRow.z == 0) {
        discard;
    }

    colRow.y *= colRow.z;

    //Calculate the pixel of the tile we want to paint
    vec2 pixel = TexCoord * ntilesMap;
    vec2 tilePos = pixel - floor(pixel);

    //Calculate the position of the atlas
    vec2 atlasPos = (tilePos + colRow.xy) / ntilesAtlas;

    FragColor = texture(tileAtlas, atlasPos);
}