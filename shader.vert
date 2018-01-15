#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform mat4 MVI;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

float getHeight(vec2 p) {
    vec3 c = texture(rgbTexture, p).xyz;
    return (heightFactor * (0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z));
}

vec3 getTriangleNormal(vec3 a, vec3 b, vec3 c) {
    vec3 v1 = c - a;
    vec3 v2 = b - a;
    return normalize(cross(v1,v2));
}

vec2 getTextureCoords(vec3 p) {
    return vec2(1.0-(p.x/widthTexture), 1.0-(p.z/heightTexture));
}

bool inGrid(vec3 p) {
    bool res = (position.x >= 0 && position.x <= widthTexture) && (position.z >= 0 && position.z <= heightTexture);
    return res;
}


void main()
{
    // get texture value, compute height
    textureCoordinate = getTextureCoords(position);
    vec3 worldPos = position;
    worldPos.y = getHeight(textureCoordinate);
    
    vec3 vertexVCS = vec3(MV * vec4(worldPos, 1.0));
    vec3 lightPosMV = vec3(MV * vec4(lightPosition,1.0));
    ToLightVector = normalize((lightPosMV - vertexVCS).xyz);
    ToCameraVector = normalize((- vertexVCS).xyz);

    vertexNormal = vec3(0,0,0);
    
    vec3 right = vec3(position.x + 1.0, position.y, position.z);
    if(inGrid(right)) {
        right.y = getHeight(getTextureCoords(right));
        vec3 up = vec3(position.x, position.y, position.z + 1.0);
        if(inGrid(up)) {
            up.y = getHeight(getTextureCoords(up));
            vertexNormal += getTriangleNormal(worldPos, right, up);
        }
        vec3 down = vec3(position.x, position.y, position.z - 1.0);
        if(inGrid(down)) {
            down.y = getHeight(getTextureCoords(down));
            vertexNormal += getTriangleNormal(worldPos, down, right);
        }
    }

    vec3 left = vec3(position.x - 1.0, position.y, position.z);
    if(inGrid(left)) {
        left.y = getHeight(getTextureCoords(left));
        vec3 up = vec3(position.x, position.y, position.z + 1.0);
        if(inGrid(up)) {
            up.y = getHeight(getTextureCoords(up));
            vertexNormal += getTriangleNormal(worldPos, up, left);
        }
        vec3 down = vec3(position.x, position.y, position.z - 1.0);
        if(inGrid(down)) {
            down.y = getHeight(getTextureCoords(down));
            vertexNormal += getTriangleNormal(worldPos, left, down);
        }
    }

    vertexNormal = normalize(vec3(MVI* vec4(normalize(vertexNormal), 0.0)));
    //////////////////////////////////
    // COMPUTE NORMALS FOR LIGHTING
    ////////////////////////////////    
    /*vec2 left = vec2(texCoods.x - (1.0/widthTexture), texCoods.y); 
    left.x = clamp(left.x, 0.0, 1.0);
    left.y = clamp(left.y, 0.0, 1.0);

    vec2 right = vec2(texCoods.x + (1.0/widthTexture), texCoods.y);
    right.x = clamp(right.x, 0.0, 1.0);
    right.y = clamp(right.y, 0.0, 1.0);

    vec2 up = vec2(texCoods.x, texCoods.y - (1.0/heightTexture));
    up.x = clamp(up.x, 0.0, 1.0);
    up.y = clamp(up.y, 0.0, 1.0);
    
    vec2 down = vec2(texCoods.x, texCoods.y + (1.0/heightTexture));
    down.x = clamp(down.x, 0.0, 1.0);
    down.y = clamp(down.y, 0.0, 1.0);

    float lh = getHeight(left);
    float rh = getHeight(right);
    float uh = getHeight(up);
    float dh = getHeight(down);
    float scaleXZ = 2.0 * ((1.0/widthTexture)/(1.0/heightTexture));

    vec3 normal = vec3(lh-rh, scaleXZ, uh-dh);
    vertexNormal = normalize(vec3(MVI*vec4(normalize(normal), 0.0)));
    */
    gl_Position = MVP * vec4(worldPos, 1.0f);
}
