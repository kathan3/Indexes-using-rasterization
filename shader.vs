#version 460 core


uniform int targetKey;  // The primary key to query
uniform int columnIndex; // The column index to query
uniform sampler2D nationTexture;  // 2D texture containing nation table data

out int queryResult;


void main() {
    // Get the texture coordinate of the current fragment
    vec4 texel = texture(nationTexture, vec2(targetKey, columnIndex));

    // Extract the primary key from the texture
    int primaryKey = int(texel.r * 255.0);

    // Check if the primary key matches the target key
    if (primaryKey == targetKey) {
        queryResult = int(texel.g * 255.0);
    } else {
        queryResult = -1;
    }
}
